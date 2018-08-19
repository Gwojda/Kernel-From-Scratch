#include "syscall.h"

void	sys_restart()
{
	//printk("-");
	reboot();
}
