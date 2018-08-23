#include "tty.h"

void	tty_write(struct s_tty *term, const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		tty_putchar(term, data[i]);
}
