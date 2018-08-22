#include "vga.h"
#include "io.h"

char	getc(void)
{
	unsigned int cr;

	cr = 0;
	if (stream_read(&keybord_stream, (char*)&cr, 1) == 1)
		return cr;
	return 0;
}
