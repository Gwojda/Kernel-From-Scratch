#ifndef SHELL_H
# define SHELL_H

# include "typedef.h"
# include "io.h"

#define SIZE_MAX_CMD	128

void	reboot(void);
void	halt(void);

typedef struct	s_execmd
{
	char	*match;
	void	(*f)();
}				t_execmd;

#endif
