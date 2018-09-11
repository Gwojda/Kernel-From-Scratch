#include "tty.h"
#include "vga.h"

void	tty_render_vga(struct s_tty *term)
{
	for (size_t y = 0; y < VGA_HEIGHT; ++y)
	{
		for (size_t x = 0; x < VGA_WIDTH; ++x)
		{
			const size_t pos = (y * VGA_WIDTH) + x;
			vga_buffer[pos] = term->screen[pos];
		}
	}
}
