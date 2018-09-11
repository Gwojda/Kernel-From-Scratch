#include "vga.h"
#include "tty.h"
#include "lib.h"

void	tty_scrollup(struct s_tty *term)
{
	size_t	i = 0;

	memmove((void *)term->screen, (const void *)(term->screen + VGA_WIDTH), VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(*term->screen));
	while (i < VGA_WIDTH)
	{
		term->screen[VGA_WIDTH * (VGA_HEIGHT - 1) + i] = vga_entry(' ', term->tty_color);
		++i;
	}
}
