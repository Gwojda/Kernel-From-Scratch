#include "terminal.h"

const size_t VGA_WIDTH = 80;
const size_t VGA_HEIGHT = 25;

void	terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	terminal_clear_screen();
}
