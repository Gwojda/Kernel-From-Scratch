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

	//mem_tester();
	process_tester();

	struct process *p, *pk;
	void user3();
	process_memory_switch(p = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12), 0);
	process_get_with_pid(0);
	pid_t pid = fork(p);
	pk = process_get_with_pid(pid);
	printk("pid = %d ptr = %p\n", pid, pk);
	int status;
	printk("wait ret = %d %d\n", process_wait(p, pid, &status, WNOHANG), status);
	exit(pk, 9);
	printk("wait ret = %d %d\n", process_wait(p, pid, &status, WNOHANG), status);
	printk("wait ret = %d %d\n", process_wait(p, pid, &status, WNOHANG), status);
	printk("wait %d %d\n", process_wait(p, -1, &status, 0), status);
	printk("wait %d %d\n", process_wait(p, pid, &status, 0), status);
	printk("wait %d %d\n", process_wait(p, pid, &status, 0), status);

	exit(p, 3);

	asm volatile("sti");
	while (1)
		asm("hlt");
}
