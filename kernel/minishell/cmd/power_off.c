#include "shell.h"
#include "acpi.h"
#include "lib.h"
#include "io.h"
#include "position.h"
#include "page.h"
#include "tty.h"

int	copy_with_physical(unsigned char *structt, void *addr, size_t struct_size)
{
	size_t delta = (size_t)addr % (1 << 12);
	size_t to_copy;
	addr = (void *)(((size_t)addr >> 12) << 12);
	while (struct_size != 0)
	{
		if (access_table_with_physical(page_swap2, addr))
			return -1;
		to_copy = struct_size;
		if (to_copy > (1 << 12) - delta)
			to_copy = (1 << 12) - delta;
		memcpy(structt, (char*)page_swap2 + (size_t)delta, to_copy);
		delta = 0;
		struct_size -= to_copy;
		structt += to_copy;
		addr += 1 << 12;
	}
	return 0;
}

// ebda = Extended BIOS Data Area
void	*get_ebda(void)
{
	// at address 0x40E in the BIOS Data Area : EBDA base address >> 4 (usually!)
	int ebda = *((short*) KERNEL_GET_VIRTUAL(0x40E));   // get pointer
	return ((void *)((ebda << 4) & 0x000FFFFF));   // transform segment into linear address
}

// check if the given address has a valid header
unsigned int *acpiCheckRSDPtr(unsigned int *ptr)
{
	const char *sig = "RSD PTR ";
	struct RSDPDescriptor *rsdp = (struct RSDPDescriptor *)ptr;
	char *bptr;
	char check = 0;

	if (memcmp(sig, rsdp, 8) == 0)
	{
		// check checksum rsdpd
		bptr = (char*) ptr;
		for (size_t i = 0; i < sizeof(struct RSDPDescriptor); i++)
		{
			check += *bptr;
			bptr++;
		}
		// found valid rsdpd
		if (check == 0)
		{
			//	printk(" ret page map = %d\n", page_map((rsdp->RsdtAddress >> 12) << 12, KERNEL_GET_VIRTUAL((rsdp->RsdtAddress >> 12) << 12), PAGE_PRESENT | PAGE_WRITE));
			return (unsigned int *) (rsdp->RsdtAddress);
		}
	}
	return NULL;
}

// finds the acpi header and returns the address of the rsdt
unsigned int *acpiGetRSDPtr(void)
{
	unsigned int *addr;
	unsigned int *rsdp;

	// search below the 1mb mark for RSDP signature
	// bios not extended research
	for (addr = (unsigned int*) KERNEL_GET_VIRTUAL(0x000E0000); (int)addr < (int)KERNEL_GET_VIRTUAL(0x00100000); addr += 0x10 / sizeof(addr))
	{
		rsdp = acpiCheckRSDPtr(addr);
		if (rsdp != NULL)
			return rsdp;
	}

	// TODO doit etre mappé ici
	// bios extended research
	int ebda = (int)KERNEL_GET_VIRTUAL(get_ebda());
	// search Extended BIOS Data Area for the Root System Description Pointer signature
	for (addr = (unsigned int*) ebda; (int) addr < ebda + 1024; addr += 0x10 / sizeof(addr))
	{
		rsdp = acpiCheckRSDPtr(addr);
		if (rsdp != NULL)
			return rsdp;
	}
	return NULL;
}

// checks for a given header and validates checksum
int acpiCheckHeader(unsigned int *ptr, char *sig)
{
	if (memcmp(ptr, sig, 4) == 0)
	{
		char *checkPtr = (char *) ptr;
		int len = *(ptr + 1);
		char check = 0;
		while (0<len--)
		{
			check += *checkPtr;
			checkPtr++;
		}
		if (check == 0)
			return 0;
	}
	return -1;
}


void *get_ACPIHeader(void *addr)
{
	struct ACPISDTHeader *h1 = kmalloc(sizeof(struct ACPISDTHeader));
	if (h1 == NULL)
		return NULL;
	copy_with_physical((unsigned char *)h1, addr, sizeof(struct ACPISDTHeader));
	struct ACPISDTHeader *h = kmalloc(sizeof(struct ACPISDTHeader) + h1->Length);
	if (h == NULL)
	{
		kfree(h1);
		return NULL;
	}
	copy_with_physical((unsigned char *)h, addr, sizeof(struct ACPISDTHeader) + h1->Length);
	kfree(h1);
	return h;
}

int initAcpi(void)
{
	struct RSDT *rsbt = get_ACPIHeader(acpiGetRSDPtr());
	int ret = -1;
	// check if address is correct (if acpi is available on this pc)
	if (rsbt != NULL && acpiCheckHeader((unsigned int *)rsbt, "RSDT") == 0)
	{
		size_t entries = (rsbt->h.Length - sizeof(rsbt->h)) / 4;
		for (size_t i = 0; i < entries; i++)
		{
			struct FACP *facp = get_ACPIHeader((void *)rsbt->PointerToOtherSDT[i]);

			if (acpiCheckHeader((unsigned int *)facp, "FACP") == 0)
			{
				struct FADT *dsdt = get_ACPIHeader(facp->DSDT);

				if (acpiCheckHeader((unsigned int *)dsdt, "DSDT") == 0)
				{
					// search the \_S5 package in the DSDT
					char *S5Addr = (char*)dsdt + 36; // skip header
					int dsdtLength = *((int *)dsdt + 1) - 36;
					while (0 < dsdtLength--)
					{
						if (memcmp(S5Addr, "_S5_", 4) == 0)
							break;
						S5Addr++;
					}
					// check if \_S5 was found
					if (dsdtLength > 0)
					{
						// check for valid AML structure
						if ((*(S5Addr - 1) == 0x08 || (*(S5Addr - 2) == 0x08 && *(S5Addr - 1) == '\\')) && *(S5Addr + 4) == 0x12)
						{
							S5Addr += 5;
							S5Addr += ((*S5Addr & 0xC0) >> 6) + 2;   // calculate PkgLength size

							if (*S5Addr == 0x0A)
								S5Addr++;   // skip byteprefix
							SLP_TYPa = *(S5Addr)<<10;
							S5Addr++;

							if (*S5Addr == 0x0A)
								S5Addr++;   // skip byteprefix
							SLP_TYPb = *(S5Addr)<<10;

							SMI_CMD = facp->SMI_CMD;

							ACPI_ENABLE = facp->ACPI_ENABLE;
							ACPI_DISABLE = facp->ACPI_DISABLE;

							PM1a_CNT = facp->PM1a_CNT_BLK;
							PM1b_CNT = facp->PM1b_CNT_BLK;

							PM1_CNT_LEN = facp->PM1_CNT_LEN;

							SLP_EN = 1<<13;
							SCI_EN = 1;

							ret = 0;
							kfree(dsdt);
							kfree(facp);
							goto end;
						} else {
							printk("\\_S5 parse error.\n");
						}
					} else {
						printk("\\_S5 not present.\n");
					}
				}
				else
				{
					printk("DSDT invalid.\n");
				}
				kfree(dsdt);
			}

			kfree(facp);

		}
		printk("no valid FACP present.\n");
	}
	else
	{
		printk("no acpi.\n");
	}

end:
	if (!ret)
		print_initialize_status("ACPI", TRUE);
	else
		print_initialize_status("ACPI", FALSE);
	kfree(rsbt);
	return ret;
}

int acpiEnable(void)
{
	// check if acpi is enabled
	if ( (inw((unsigned int) PM1a_CNT) &SCI_EN) == 0)
	{
		// check if acpi can be enabled
		if (SMI_CMD != 0 && ACPI_ENABLE != 0)
		{
			outb((unsigned int) SMI_CMD, ACPI_ENABLE); // send acpi enable command
			// give 3 seconds time to enable acpi
			// we can't sleep for now
			int i;
			for (i=0; i<300; i++)
			{
				if ( (inw((unsigned int) PM1a_CNT) &SCI_EN) == 1)
					break;
				//            sleep(10);
			}
			if (PM1b_CNT != 0)
				for (; i<300; i++)
				{
					if ( (inw((unsigned int) PM1b_CNT) &SCI_EN) == 1)
						break;
					//         	    sleep(10);
				}
			if (i<300) {
				printk("enabled acpi.\n");
				return 0;
			} else {
				printk("couldn't enable acpi.\n");
				return -1;
			}
		} else {
			printk("no known way to enable acpi.\n");
			return -1;
		}
	} else {
		printk("acpi was already enabled.\n");
		return 0;
	}
}

void acpiPowerOff(void)
{
	// SCI_EN is set to 1 if acpi shutdown is possible
	if (SCI_EN == 0)
		return;

	acpiEnable();
	// send the shutdown command
	outw((unsigned int) PM1a_CNT, SLP_TYPa | SLP_EN );
	if ( PM1b_CNT != 0 )
		outw((unsigned int) PM1b_CNT, SLP_TYPb | SLP_EN );

	printk("acpi poweroff failed.\n");
}
