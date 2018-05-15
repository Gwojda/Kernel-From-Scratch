#include "kernel.h"

static void	print_stack(void)
{
	uintptr_t framesp;
	uintptr_t framebp;
	char	toto[] = "salut tout le monde !\n";

	asm("movl %%ebp, %0" : "=r" (framesp));
		printk("%p\n", framesp);
	framebp = framesp;
	asm("movl %%esp, %0" : "=r" (framebp));

		printk("%p\n", framesp);
		printk("%p\n", framebp);
/*	while (framesp <= framebp)
	{
		printk("%p\n", framesp);
		printk("%p\n", framebp);
		printk("%c\n", *(char *)framesp);
		++framesp;
	}*/
}

void kmain(void)
{
	char	c;

//	init_gdt();
	init_vga();
	init_tty();
	vga_putstr("\n"
"         ___---___\n"
"      .--         --.\n"
"    ./   ()      .-. \\.\n"
"   /   o    .   (   )  \\\n"
"  / .            '-'    \\\n"
" | ()    .  O         .  |\n"
"|                         |\n"
"|    o           ()       |\n"
"|       .--.          O   |\n"
" | .   |    |            |\n"
"  \\    `.__.'    o   .  /\n"
"   \\                   /\n"
"    `\\  o    ()      /'\n"
"      `--___   ___--'\n"
"            ---\n");
	printk("Prink test ! %d \n", 4242);

//		to prove we've got multiple tty :
//	--------------------------------------
	size_t	i = 0;

	while (i < MAX_TTY)
	{
		tty[i].tty_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK + i);
		tty_clear_screen(i);
		++i;
	}

//	--------------------------------------

	print_stack();
	while (1)
	{
		c = getc();
//		to prove we've got multiple tty :
		if (isdigit(c))
			switch_tty(c - '0');
		else
			vga_putchar(c);
	}
}
