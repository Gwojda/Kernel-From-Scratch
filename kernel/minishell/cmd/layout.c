#include "shell.h"
#include "printk.h"

void layout(char *cmd)
{
	while (*cmd && *cmd != ' ')
		cmd++;
	while (*cmd && *cmd == ' ')
		cmd++;
	if (*cmd)
	{
		if (set_layout(cmd) == 0)
			printk("set layout to \"%s\"\n", cmd);
		else
			printk("no layout found \"%s\"\n", cmd);
	}
	else
		printk("current layout %s", get_layout());
}
