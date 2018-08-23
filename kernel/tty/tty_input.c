#include "tty.h"

void	tty_input_char(struct s_tty *term, char key)
{
	if (key == '\0')
		;
	else if (key == '\b')
	{
		if (term->input_raw_c > 0)
		{
			term->input_raw_c--;
			tty_putchar(term, key);
		}
	}
	else if (term->input_raw_c < STREAM_BUFF - 2 || (term->input_raw_c < STREAM_BUFF - 1 && key == '\n'))
	{
		term->input_raw[term->input_raw_c] = key;
		term->input_raw_c++;
		tty_putchar(term, key);
		if (key == '\n')
		{
			stream_write(&term->input, term->input_raw, term->input_raw_c);
			term->input_raw_c = 0;
		}
	}
}
