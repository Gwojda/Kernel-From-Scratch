#include "acpi.h"
#include "lib.h"
#include "io.h"

// ebda = Extended BIOS Data Area
void	*get_ebda(void)
{
	// at address 0x40E in the BIOS Data Area : EBDA base address >> 4 (usually!)
    int ebda = *((short*) 0x40E);   // get pointer
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
         return (unsigned int *) rsdp->RsdtAddress;
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
   for (addr = (unsigned int*) 0x000E0000; (int)addr < 0x00100000; addr += 0x10 / sizeof(addr))
   {
      rsdp = acpiCheckRSDPtr(addr);
      if (rsdp != NULL)
         return rsdp;
   }

   // bios extended research
   int ebda = (int)get_ebda();
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

int initAcpi(void)
{
   struct RSDT *rsbt = (struct RSDT *)acpiGetRSDPtr();

   // check if address is correct (if acpi is available on this pc)
   if (rsbt != NULL && acpiCheckHeader((unsigned int *)rsbt, "RSDT") == 0)
   {
	   size_t entries = (rsbt->h.Length - sizeof(rsbt->h)) / 4;
	   for (size_t i = 0; i < entries; i++)
	   {
		   if (acpiCheckHeader((unsigned int *)rsbt->PointerToOtherSDT[i], "FACP") == 0)
		   {
			   struct FACP *facp = (struct FACP *)rsbt->PointerToOtherSDT[i];
			   if (acpiCheckHeader((unsigned int *)facp->DSDT, "DSDT") == 0)
			   {
				   // search the \_S5 package in the DSDT
	               char *S5Addr = (char*)facp->DSDT + 36; // skip header
	               int dsdtLength = *(facp->DSDT + 1) - 36;
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

	                     return 0;
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
		   }

	   }
	   printk("no valid FACP present.\n");
   }
   else
   {
      printk("no acpi.\n");
   }

   return -1;
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
      //printk("acpi was already enabled.\n");
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
