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
		syscall(0);
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
		syscall(0);
	}
}

__attribute__ ((section(".ucode"))) void user_shell(void)
{
	launchshell();
}
