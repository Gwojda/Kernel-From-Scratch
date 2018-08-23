#include "tty.h"
#include "vga.h"
#include "lib.h"

static void	tty_printchar(struct s_tty *term, char c)
{
	tty_putentryat(term, c, term->tty_color, term->tty_column, term->tty_row);
	if (++term->tty_column == VGA_WIDTH)
	{
		term->tty_column = 0;
		if (term->tty_row != VGA_HEIGHT)
			++term->tty_row;
		if (term->tty_row == VGA_HEIGHT)
		{
			tty_scrollup(term);
			if (term == current_tty)
				tty_render_vga(term);
			--term->tty_row;
		}
	}
	if (term == current_tty)
		vga_move_cursor(term->tty_column, term->tty_row);
}

static void	tty_putendl(struct s_tty *term)
{
	if (term->tty_row != VGA_HEIGHT)
		++term->tty_row;
	if (term->tty_row == VGA_HEIGHT)
	{
		tty_scrollup(term);
		if (term == current_tty)
			tty_render_vga(term);
		--term->tty_row;
	}
	term->tty_column = 0;
	if (term == current_tty)
		vga_move_cursor(term->tty_column, term->tty_row);
}

static void	tty_backspace(struct s_tty *term)
{
	if (term->tty_column == 0 && term->tty_row)
	{
		term->tty_column = VGA_WIDTH - 1;
		--term->tty_row;
	}
	else
		--term->tty_column;
	tty_putentryat(term, ' ', term->tty_color, term->tty_column, term->tty_row);
	if (term == current_tty)
		vga_move_cursor(term->tty_column, term->tty_row);
}

static void	tty_putretchariot(struct s_tty *term)
{
	term->tty_column = 0;
}

void	tty_putchar(struct s_tty *term, char c)
{
	if (isprint(c))
		tty_printchar(term, c);
	else if (c == '\n')
		tty_putendl(term);
	else if (c == '\r')
		tty_putretchariot(term);
	else if (c == '\b')
		tty_backspace(term);
}
