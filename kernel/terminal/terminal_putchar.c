#include "terminal.h"

static void	terminal_printchar(void)
{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH)
	{
		terminal_column = 0;
		if (terminal_row != VGA_HEIGHT)
			++terminal_row;
		if (terminal_row == VGA_HEIGHT)
			terminal_scrollup();
	}
}

static void	terminal_putendl(void)
{
	if (terminal_row != VGA_HEIGHT)
		++terminal_row;
	if (terminal_row == VGA_HEIGHT)
		terminal_scrollup();
	terminal_column = 0;
}

static void	terminal_putretchariot(void)
{
	terminal_column = 0;
}

void	terminal_putchar(char c)
{
	if (isprint(c))
		terminal_printchar();
	else if (c == '\n')
		terminal_putendl();
	else if (c == '\r')
		terminal_putretchariot();
}
