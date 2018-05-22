#include "shell.h"
#include "printk.h"
#include "lib.h"

void	*get_ebp(void);
void	*get_esp(void);

static void	print_stack(void)
{
	char	t = 't';
	char	u = 'u';
	char	l = 'l';
	char	a = 'a';
	char	s = 's';
	void	*framesp = get_esp();
	void	*framebp = get_ebp();

	printk("%p\n", framesp);
	printk("%p\n", framebp);
	while (framesp <= framebp)
	{
		printk("%c\n", *(char *)framesp);
		++framesp;
	}
}

static void	pushback_char(char cmd[SIZE_MAX_CMD], size_t *cmd_ind, char c)
{
	if ((*cmd_ind) + 1 == SIZE_MAX_CMD)
		return ;
	cmd[*cmd_ind] = c;
	++(*cmd_ind);
	cmd[*cmd_ind] = '\0';
}

static void	clear_cmd(char cmd[SIZE_MAX_CMD], size_t *cmd_ind)
{
	*cmd_ind = 0;
	cmd[*cmd_ind] = '\0';
}

static void	exec_cmd(char cmd[SIZE_MAX_CMD])
{
	static t_execmd	correstab[] =
	{
		{"reboot"	, reboot},
		{"halt"		, halt},
	};
	size_t	correstab_size = sizeof(correstab) / sizeof(t_execmd);
	size_t	i = 0;

	while (i < correstab_size)
	{
		if (!strcmp(correstab[i].match, (char *)cmd))
			correstab[i].f();
		++i;
	}
}

void	launchshell(void)
{
	char	cmd[SIZE_MAX_CMD];
	char	c;
	size_t	cmd_ind = 0;

	while (1)
	{
		c = getc();
		if (c == '\n')
		{
			exec_cmd(cmd);
			clear_cmd(cmd, &cmd_ind);
		}
		else
			pushback_char(cmd, &cmd_ind, c);
		vga_putchar(c);
	}
}
