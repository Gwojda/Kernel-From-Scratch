#include "tty.h"
#include "vga.h"

void	vga_setcolor(uint8_t color)
{
	tty[current_tty].tty_color = color;
}
