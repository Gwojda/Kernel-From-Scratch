#include "vga.h"
#include "tty.h"
#include "lib.h"

void	tty_scrollup(void)
{
	size_t	i = 0;

	memmove((void *)tty[current_tty].screen, (const void *)(tty[current_tty].screen + VGA_WIDTH), VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(*tty[current_tty].screen));
	while (i < VGA_WIDTH)
	{
		tty[current_tty].screen[VGA_WIDTH * (VGA_HEIGHT - 1) + i] = vga_entry(' ', tty[ind].tty_color);
		++i;
	}
}
