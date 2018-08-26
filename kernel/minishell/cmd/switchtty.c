#include "shell.h"
#include "tty.h"
#include "lib.h"

void switchtty_cmd(void)
{
	const char		default_cmd[] = "switchtty ";
	const size_t	default_cmd_size = sizeof(default_cmd) / sizeof(char);
	int				nb;

	if (strncmp(default_cmd, (char *)cmd, default_cmd_size - 1))
		return ;
	nb = atoi((char *)cmd + default_cmd_size - 1);
	if (nb < 0 || nb >= MAX_TTY)
		return ;
	switch_tty(&tty[nb]);
}
