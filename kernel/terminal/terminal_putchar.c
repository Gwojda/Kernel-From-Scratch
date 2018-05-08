#include "terminal.h"

void	terminal_putchar(char c)
{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (terminal_row != VGA_HEIGHT)
			++terminal_row;
		if (terminal_row == VGA_HEIGHT)
			terminal_scrollup();
	}
}
