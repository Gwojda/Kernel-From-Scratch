#include "vga.h"
#include "io.h"

void	vga_move_cursor(size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(index & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((index >> 8) & 0xFF));
}
