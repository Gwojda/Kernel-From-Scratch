#include "vga.h"
#include "position.h"

void	init_vga(void)
{
	vga_buffer = (uint16_t*)0xC00B8000;
	vga_buffer = (uint16_t*)(0xB8000 + KERNEL_POS);
	//vga_buffer = (uint16_t*)0xB8000;
}
