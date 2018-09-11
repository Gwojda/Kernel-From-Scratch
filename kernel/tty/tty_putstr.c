#include "tty.h"
#include "lib.h"

void	tty_putstr(struct s_tty *term, const char* data)
{
	tty_write(term, data, strlen(data));
}
