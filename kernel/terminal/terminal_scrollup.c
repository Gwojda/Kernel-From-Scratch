#include "terminal.h"

void	terminal_scrollup(void)
{
	if (terminal_row + 1 != VGA_HEIGHT || terminal_column + 1 != VGA_WIDTH)
		return ;
	memmove(terminal_buffer, terminal_buffer + VGA_WIDTH, VGA_WIDTH * (VGA_HEIGHT - 1));
}
