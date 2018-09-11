#include "tty.h"

struct s_tty	*current_tty;

void	init_tty(void)
{
	size_t	i = 0;

	current_tty = &tty[0];
	while (i < MAX_TTY)
	{
		tty[i].tty_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK + i);
		tty[i].input_raw_c = 0;
		tty_clear_screen(&tty[i]);
		++i;
	}
}
