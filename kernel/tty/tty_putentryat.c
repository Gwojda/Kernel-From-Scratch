#include "vga.h"
#include "tty.h"

void	tty_putentryat(struct s_tty *term, char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;

	term->screen[index] = vga_entry(c, color);
	if (current_tty == term)
		vga_buffer[index] = vga_entry(c, color);
}
