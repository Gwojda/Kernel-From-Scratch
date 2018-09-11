#include "kernel.h"
#include "process.h"

static void	print_kernel_visu(void)
{
	tty_setcolor(&tty[0], vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
	tty_putstr(&tty[0], "\n"
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
	tty_setcolor(&tty[0], vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

}

void user_shell(void);

void kmain (unsigned long volatile magic, unsigned long addr)
{
	void*	esp;

	init_tty();
	init_vga();
	memory_init(magic, addr);

	print_kernel_visu();


	init_gdt();
	page_setup();
	stack_setup();

	__asm__ volatile ("movl %%esp, %[r]" :  [r] "=r" (esp));
	esp = STACK_END - 1 - ((void*)&stack_top - esp);
	__asm__ volatile ("movl %[r], %%esp" : : [r] "r" (esp));
	heap_setup();
	print_initialize_status("Memory setup", TRUE);
	init_idt();
	init_process();

//	mem_tester();
	process_tester();

	struct process *p = process_ini_kern((u32*)user_shell, (void*)user_shell + 0xC0000000, 1 << 12);
	process_memory_switch(p, 0);
	asm volatile("sti");
	while (1)
		asm("hlt");
}
