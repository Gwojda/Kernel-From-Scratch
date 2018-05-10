#include "kernel.h"

void kmain(void)
{
	size_t i = 0;
	/* Initialize terminal interface */
	terminal_initialize();

	/* Newline support is left as an exercise. */
	terminal_color = vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
	terminal_putstr("\
         ___---___\
      .--         --.\
    ./   ()      .-. \\.\
   /   o    .   (   )  \\\
  / .            '-'    \\\
 | ()    .  O         .  |\
|                         |\
|    o           ()       |\
|       .--.          O   |\
 | .   |    |            |\
 \\    `.__.'    o   .  /\
  \\                   /\
   `\\  o    ()      /'\
      `--___   ___--'\
            ---\n");
	while (1)
		print_keyboard_entry();
}
