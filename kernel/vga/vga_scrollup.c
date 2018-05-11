#include "vga.h"
#include "lib.h"

void	vga_scrollup(void)
{
	memmove((void *)vga_buffer, (const void *)(vga_buffer + VGA_WIDTH), VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(*vga_buffer));
	bzero((void *)(vga_buffer + (VGA_WIDTH * (VGA_HEIGHT - 1))), VGA_WIDTH * sizeof(*vga_buffer));
}
