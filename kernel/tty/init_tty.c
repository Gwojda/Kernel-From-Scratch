#include "tty.h"

size_t	current_tty = 0;

void	init_tty(void)
{
	size_t	i = 0;

	while (i < MAX_TTY)
	{
		tty[i].tty_row = 0;
		tty[i].tty_column = 0;
		tty[i].tty_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		tty_clear_screen(i);
		++i;
	}
}
