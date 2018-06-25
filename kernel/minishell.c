#include "shell.h"
#include "printk.h"
#include "lib.h"

size_t	cmd_ind = 0;
char cmd[SIZE_MAX_CMD];

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

static void	remove_last_char(void)
{
	cmd[cmd_ind] = '\0';
	if (cmd_ind)
	{
		--cmd_ind;
		vga_putchar('\b');
	}
}

static void	pushback_char(char c)
{
	if (cmd_ind + 1 == SIZE_MAX_CMD)
		return ;
	if (c == '\b')
		remove_last_char();
	else
	{
		vga_putchar(c);
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
		{"reboot"		, reboot		},
		{"halt"			, halt			},
		{"switchtty"	, switchtty_cmd	}
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
