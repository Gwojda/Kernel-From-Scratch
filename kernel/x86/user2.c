#include "syscall.h"
#include "shell.h"

__attribute__ ((section(".ucode"))) void user2(void)
{
	int i;

	while (1)
	{
		i = 0;
		while (i++ < 10000000 / 2)
			;
		asm("mov $0x00, %eax; int $0x80");
		//sys();
	}
}

__attribute__ ((section(".ucode"))) void user3(void)
{
	int i;

	while (1)
	{
		i = 0;
		while (i++ < 10000000 / 2)
			;
		asm("mov $0x00, %eax; int $0x80");
		//sys();
	}
}

__attribute__ ((section(".ucode"))) void sys(void)
{
	asm("mov $0x00, %eax; int $0x80");
}

__attribute__ ((section(".ucode"))) void user_shell(void)
{
	launchshell();
}
