#include "tty.h"
#include "vga.h"

void	tty_clear_screen(struct s_tty *term)
{
	for (size_t y = 0; y < VGA_HEIGHT; ++y)
	{
		for (size_t x = 0; x < VGA_WIDTH; ++x)
			term->screen[(y * VGA_WIDTH) + x] = vga_entry(' ', term->tty_color);
	}
	term->tty_row = 0;
	term->tty_column = 0;
	if (current_tty == term)
	{
		tty_render_vga(term);
		vga_move_cursor(0, 0);
	}
}
