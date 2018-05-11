#include "tty.h"
#include "vga.h"

void	tty_clear_screen(size_t ind)
{
	for (size_t y = 0; y < VGA_HEIGHT; ++y)
	{
		for (size_t x = 0; x < VGA_WIDTH; ++x)
			tty[ind].screen[(y * VGA_WIDTH) + x] = vga_entry(' ', tty[ind].tty_color);
	}
}
