#include "tty.h"
#include "vga.h"

void	tty_clear_screen(size_t ind)
{
	for (size_t y = 0; y < VGA_HEIGHT; ++y)
	{
		for (size_t x = 0; x < VGA_WIDTH; ++x)
			tty[ind].screen[(y * VGA_WIDTH) + x] = vga_entry(' ', tty[ind].tty_color);
	}
	tty[ind].tty_row = 0;
	tty[ind].tty_column = 0;
	if (current_tty == ind)
	{
		vga_render_tty();
		vga_move_cursor(0, 0);
	}
}
