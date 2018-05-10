#include "terminal.h"
#include "lib.h"

void	terminal_scrollup(void)
{
	memmove((void *)terminal_buffer, (const void *)(terminal_buffer + VGA_WIDTH), VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(terminal_buffer));
	bzero((void *)(terminal_buffer + (VGA_WIDTH * (VGA_HEIGHT - 1))), VGA_WIDTH * sizeof(terminal_buffer));
}
