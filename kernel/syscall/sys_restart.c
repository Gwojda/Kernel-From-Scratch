#include "syscall.h"

void	sys_restart(long p1, long p2, long p3, long p4, long p5)
{
	printk("p1 %d\n", p1);
	printk("p2 %d\n", p2);
	printk("p3 %d\n", p3);
	printk("p4 %d\n", p4);
	printk("p5 %d\n", p5);
//	reboot();
}
