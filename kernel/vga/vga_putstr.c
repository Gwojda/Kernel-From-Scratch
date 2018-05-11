#include "vga.h"
#include "lib.h"

void	vga_putstr(const char* data)
{
	vga_write(data, strlen(data));
}
