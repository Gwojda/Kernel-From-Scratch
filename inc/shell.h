#ifndef SHELL_H
# define SHELL_H

# include <stddef.h>
# include <stdint.h>

# include "typedef.h"
# include "io.h"
# include "asm.h"

#define SIZE_MAX_CMD	128

void	switchtty_cmd(void);
void	reboot(void);
void	halt(void);

char cmd[SIZE_MAX_CMD];
extern size_t	cmd_ind;

typedef struct	s_execmd
{
	char	*match;
	void	(*f)();
}				t_execmd;

#endif
