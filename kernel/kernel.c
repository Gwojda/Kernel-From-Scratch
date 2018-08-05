#include "kernel.h"

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

void kmain (unsigned long volatile magic, unsigned long addr)
{
	void*	esp;

	init_tty();
	init_vga();
	memory_init(magic, addr);

// ------------------------

	print_kernel_visu();

	printk("Kernel size %p\n", KERNEL_SIZE);
	printk("Kernel TEXT size %p\n", KERNEL_TEXT_SIZE);
	printk("Kernel BSS size %p\n", KERNEL_BSS_SIZE);
	printk("Kernel RODATA size %p\n", KERNEL_RODATA_SIZE);
	printk("Kernel DATA size %p\n", KERNEL_DATA_SIZE);
// ------------------------

	page_setup();
	stack_setup();

	__asm__ volatile ("movl %%esp, %[r]" :  [r] "=r" (esp));
	esp = STACK_END - 1 - ((void*)&stack_top - esp);
	__asm__ volatile ("movl %[r], %%esp" : : [r] "r" (esp));

	init_gdt();
//	launchshell();
	heap_setup();
	short	*ptr = kmalloc(16);
	printk("ptr = %p\n", ptr);
	*ptr = 12;
	printk("*ptr = %d\n", *ptr);
	short	*ptr2 = kmalloc(16);
	printk("ptr2 = %p\n", ptr2);
	*ptr2 = 12;
	printk("*ptr2 = %d\n", *ptr2);
	short	*ptr3 = kmalloc(16);
	printk("ptr3 = %p\n", ptr3);
	*ptr3 = 12;
	printk("*ptr3 = %d\n", *ptr3);
}
