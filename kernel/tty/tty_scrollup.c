#include "vga.h"
#include "tty.h"
#include "lib.h"

void	tty_scrollup(void)
{
	memmove((void *)tty[current_tty].screen, (const void *)(tty[current_tty].screen + VGA_WIDTH), VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(*tty[current_tty].screen));
	bzero((void *)(tty[current_tty].screen + (VGA_WIDTH * (VGA_HEIGHT - 1))), VGA_WIDTH * sizeof(*tty[current_tty].screen));
}
