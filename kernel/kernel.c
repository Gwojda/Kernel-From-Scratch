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
	short	*ptr;
	for (size_t i = 0; i < 4096; i++)
	{
		ptr = kmalloc(16);
		printk("ptr = %p\n", ptr);
		*ptr = 42;
		kfree(ptr);
	}
/*	short	*ptr4 = kmalloc(4000);
	printk("ptr4 = %p\n", ptr4);
	for(size_t i = 0;i < 4000; ++i)
		((char *)ptr4)[i] = 42;
	for (size_t i = 0; i < 2048; i++)
	{
		ptr = kmalloc(16);
		*ptr = 42;
	}
	ptr4 = kmalloc(4000);
	printk("ptr4 = %p\n", ptr4);
	for(size_t i = 0;i < 4000; ++i)
		((char *)ptr4)[i] = 42;
	ptr4 = kmalloc(4000);
	printk("ptr4 = %p\n", ptr4);
	for(size_t i = 0;i < 4000; ++i)
		((char *)ptr4)[i] = 42;*/
	printk("FINISH\n");
}
