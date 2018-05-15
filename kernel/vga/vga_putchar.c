#include "tty.h"
#include "vga.h"
#include "lib.h"

static void	vga_printchar(char c)
{
	vga_putentryat(c, tty[current_tty].tty_color, tty[current_tty].tty_column, tty[current_tty].tty_row);
	if (++tty[current_tty].tty_column == VGA_WIDTH)
	{
		tty[current_tty].tty_column = 0;
		if (tty[current_tty].tty_row != VGA_HEIGHT)
			++tty[current_tty].tty_row;
		if (tty[current_tty].tty_row == VGA_HEIGHT)
		{
			tty_scrollup();
			vga_render_tty();
			--tty[current_tty].tty_row;
		}
	}
	vga_move_cursor(tty[current_tty].tty_column, tty[current_tty].tty_row);
}

static void	vga_putendl(void)
{
	if (tty[current_tty].tty_row != VGA_HEIGHT)
		++tty[current_tty].tty_row;
	if (tty[current_tty].tty_row == VGA_HEIGHT)
	{
		tty_scrollup();
		vga_render_tty();
		--tty[current_tty].tty_row;
	}
	tty[current_tty].tty_column = 0;
	vga_move_cursor(tty[current_tty].tty_column, tty[current_tty].tty_row);
}

static void	vga_backspace(void)
{
	if (tty[current_tty].tty_column && --tty[current_tty].tty_column == 0 && tty[current_tty].tty_row)
	{
		tty[current_tty].tty_column = VGA_WIDTH - 1;
		--tty[current_tty].tty_row;
	}
	vga_putentryat(' ', tty[current_tty].tty_color, tty[current_tty].tty_column, tty[current_tty].tty_row);
	vga_move_cursor(tty[current_tty].tty_column, tty[current_tty].tty_row);
}

static void	vga_putretchariot(void)
{
	tty[current_tty].tty_column = 0;
}

void	vga_putchar(char c)
{
	if (isprint(c))
		vga_printchar(c);
	else if (c == '\n')
		vga_putendl();
	else if (c == '\r')
		vga_putretchariot();
	else if (c == '\b')
		vga_backspace();
}
