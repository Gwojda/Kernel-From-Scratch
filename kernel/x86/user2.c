#include "syscall.h"
#include "shell.h"

__attribute__ ((section(".ucode"))) void user2(void)
{
	volatile int j = 42;
	int i;
	volatile int k = 42;

	while (1)
	{
		i = 0;
		while (i++ < 10000000 / 2)
			;
		//asm("mov $0x00, %eax; int $0x80");
		if (j != k)
			while (1)
				;
		sys();
	}
}

__attribute__ ((section(".ucode"))) void user3(void)
{
	volatile int j = 42;
	int i;
	volatile int k = 42;

	while (1)
	{
		i = 0;
		while (i++ < 10000000 / 2)
			;
		//asm("mov $0x00, %eax; int $0x80");
		if (j != k)
			while (1)
				;
		sys();
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
