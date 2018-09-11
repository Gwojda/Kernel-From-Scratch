#include "tty.h"

void	tty_setcolor(struct s_tty *term, uint8_t color)
{
	term->tty_color = color;
}
