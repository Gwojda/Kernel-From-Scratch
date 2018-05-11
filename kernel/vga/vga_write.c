#include "vga.h"

void	vga_write(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		vga_putchar(data[i]);
}
