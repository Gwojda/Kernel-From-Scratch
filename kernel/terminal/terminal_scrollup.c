#include "terminal.h"
#include "lib.h"

void	terminal_scrollup(void)
{
	memcpy((void *)terminal_buffer, (const void *)(terminal_buffer + VGA_WIDTH), VGA_WIDTH * (VGA_HEIGHT - 1));
	bzero((void *)(terminal_buffer + (VGA_WIDTH * (VGA_HEIGHT - 1))), VGA_WIDTH);
}
