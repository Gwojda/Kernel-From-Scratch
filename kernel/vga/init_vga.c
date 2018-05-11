#include "vga.h"

void	init_vga(void)
{
	vga_buffer = (uint16_t*)0xB8000;
}
