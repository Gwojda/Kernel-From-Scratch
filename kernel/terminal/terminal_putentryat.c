#include "terminal.h"

void	terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;

	if (c == '\n')
	{
		++terminal_row;
		terminal_column = 0;
	}
	else
		terminal_buffer[index] = vga_entry(c, color);
}
