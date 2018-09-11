#include "vga.h"
#include "io.h"
#include "tty.h"

char	getc(void)
{
	unsigned int cr;

	cr = 0;
	if (stream_read(&current_tty->input, (char*)&cr, 1) == 1)
		return cr;
	return 0;
}
