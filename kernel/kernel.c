#include "kernel.h"
#include "multiboot2.h"

static void	print_kernel_visu(void)
{
	vga_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK + current_tty));
	vga_putstr("\n"
			"                             ___---___\n"
			"                          .--         --.\n"
			"                        ./   ()      .-. \\.\n"
			"                       /   o    .   (   )  \\\n"
			"                      / .            '-'    \\\n"
			"                     | ()    .  O         .  |\n"
			"                    |                         |\n"
			"                    |    o           ()       |\n"
			"                    |       .--.          O   |\n"
			"                     | .   |    |            |\n"
			"                      \\    `.__.'    o   .  /\n"
			"                       \\                   /\n"
			"                        `\\  o    ()      /'\n"
			"                          `--___   ___--'\n"
			"                         Moonlight Kernel\n"
			"################################################################################\n"
			);
	vga_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK + current_tty));

}

void kmain (unsigned long magic, unsigned long addr)
{
	struct multiboot_tag *tag;
	unsigned size;

	init_tty();
	init_vga();
	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
	{
		printk ("Invalid magic number: 0x%x\n", (unsigned) magic);
		return;
	}
	if (addr & 7)
	{
		printk ("Unaligned mbi: 0x%x\n", addr);
		return;
	}
	size = *(unsigned *) addr;
	printk ("Announced mbi size 0x%x 0x%x\n", size, addr);
	for (tag = (struct multiboot_tag *) (addr + 8);
			tag->type != MULTIBOOT_TAG_TYPE_END;
			tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
				+ ((tag->size + 7) & ~7)))
	{
		printk ("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
		switch (tag->type)
		{
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				printk ("Command line = %s\n",
						((struct multiboot_tag_string *) tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				printk ("Boot loader name = %s\n",
						((struct multiboot_tag_string *) tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_MODULE:
				printk ("Module at 0x%x-0x%x. Command line %s\n",
						((struct multiboot_tag_module *) tag)->mod_start,
						((struct multiboot_tag_module *) tag)->mod_end,
						((struct multiboot_tag_module *) tag)->cmdline);
				break;
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
				printk ("mem_lower = %uKB, mem_upper = %uKB\n",
						((struct multiboot_tag_basic_meminfo *) tag)->mem_lower,
						((struct multiboot_tag_basic_meminfo *) tag)->mem_upper);
				break;
			case MULTIBOOT_TAG_TYPE_BOOTDEV:
				printk ("Boot device 0x%x,%u,%u\n",
						((struct multiboot_tag_bootdev *) tag)->biosdev,
						((struct multiboot_tag_bootdev *) tag)->slice,
						((struct multiboot_tag_bootdev *) tag)->part);
				break;
			case MULTIBOOT_TAG_TYPE_MMAP:
				{
					multiboot_memory_map_t *mmap;
					size_t			tot_length = 0;
					size_t			tot_free_length = 0;
					size_t			tot_used_length = 0;

					printk ("mmap\n");
					for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
							(multiboot_uint8_t *) mmap
							< (multiboot_uint8_t *) tag + tag->size;
							mmap = (multiboot_memory_map_t *)
							((unsigned long) mmap
							 + ((struct multiboot_tag_mmap *) tag)->entry_size))
					{
						printk (" base_addr = 0x%x%x,"
								" length = 0x%x%x, type = 0x%x\n",
								(unsigned) (mmap->addr >> 32),
								(unsigned) (mmap->addr & 0xffffffff),
								(unsigned) (mmap->len >> 32),
								(unsigned) (mmap->len & 0xffffffff),
								(unsigned) mmap->type);
						if (mmap->type == 1)
						{
							tot_free_length += (unsigned) (mmap->len >> 32);
							tot_free_length += (unsigned) (mmap->len & 0xffffffff);
						}
						if (mmap->type == 2)
						{
							tot_used_length += (unsigned) (mmap->len >> 32);
							tot_used_length += (unsigned) (mmap->len & 0xffffffff);
						}
						tot_length += (unsigned) (mmap->len >> 32);
						tot_length += (unsigned) (mmap->len & 0xffffffff);
					}
					printk("total : %x, total used : %x, total free : %x\n", tot_length, tot_used_length, tot_free_length);
				}
				break;
		}
	}
	tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
			+ ((tag->size + 7) & ~7));
	printk ("Total mbi size 0x%x\n", (unsigned) tag - addr);

	printk("Kernel half destination %p\n", KERNEL_POS);
	printk("Kernel virtual position %p %p\n", KERNEL_START, KERNEL_END);
	printk("Kernel real position %p %p\n", KERNEL_REAL_START, KERNEL_REAL_END);
	printk("Kernel size %p\n", KERNEL_SIZE);
	printk("Kernel virtual bss position %p %p\n", KERNEL_BSS_START, KERNEL_BSS_END);
	printk("Kernel real bss position %p %p\n", KERNEL_REAL_BSS_START, KERNEL_REAL_BSS_END);
	printk("Kernel size %p\n", KERNEL_BSS_SIZE);
	page_setup();
	//print_kernel_visu();
	init_gdt();
	launchshell();
}
