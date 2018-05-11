#include "vga.h"
#include "tty.h"

void	vga_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;

	vga_buffer[index] = vga_entry(c, color);
	tty[current_tty].screen[index] = vga_entry(c, color);
}
