#include "shell.h"
#include "printk.h"
#include "lib.h"
#include "tty.h"

size_t	cmd_ind = 0;
char cmd[SIZE_MAX_CMD];


static void	remove_last_char(void)
{
	cmd[cmd_ind] = '\0';
	if (cmd_ind)
		--cmd_ind;
}

static void	pushback_char(char c)
{
	if (cmd_ind + 1 == SIZE_MAX_CMD)
		return ;
	if (c == '\b')
		remove_last_char();
	else
	{
		cmd[cmd_ind] = c;
		++cmd_ind;
		cmd[cmd_ind] = '\0';
	}
}

static void	clear_cmd(void)
{
	cmd_ind = 0;
	cmd[cmd_ind] = '\0';
}

static void	exec_cmd(void)
{
	const t_execmd	correstab[] =
	{
		{"reboot",		reboot},
		{"halt",		halt},
		{"switchtty",		switchtty_cmd},
		{"memdebug", 		mem_debug}
	};
	const size_t	correstab_size = sizeof(correstab) / sizeof(t_execmd);
	size_t			i = 0;

	while (i < correstab_size)
	{
		if (!strncmp(correstab[i].match, (char *)cmd, strlen(correstab[i].match)))
		{
			correstab[i].f(cmd);
			break ;
		}
		++i;
	}
	if (i == correstab_size)
		printk("unknow command\n");
}

void	launchshell(void)
{
	char	c;

	printk("$> ");
	while (1)
	{
		while (!(c = getc()))
			;
		if (c == '\n')
		{
			exec_cmd();
			clear_cmd();
			printk("\n$> ");
		}
		else
			pushback_char(c);
	}
}
