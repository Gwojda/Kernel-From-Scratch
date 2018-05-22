#include "kernel.h"

void kmain(void)
{
	init_gdt();
	init_vga();
	init_tty();

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
	);

	launchshell();
}
