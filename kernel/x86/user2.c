#include "syscall.h"
#include "shell.h"
#include "process.h"
#include "errno.h"

void sys(void);

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

__attribute__ ((section(".ucode"))) void testwait(void)
{
	int status;
	pid_t pid;
start:
	pid = process_wait(current, -1, &status, WNOHANG);
	if (pid == -ECHILD)
		exit(current, 4);
	if (pid == NULL || pid == -EAGAIN)
		goto start;
	printk("status%d %d %d\n", current->pid, status, pid);
	goto start;
}

__attribute__ ((section(".ucode"))) void sys(void)
{
	asm("mov $0x00, %eax; int $0x80");
}

__attribute__ ((section(".ucode"))) void user_shell(void)
{
	launchshell();
}

__attribute__ ((section(".ucode"))) void user_noobcrash(void)
{
	char *ptr = NULL;
	*ptr = 42;	// AH
}

__attribute__ ((section(".ucode"))) void init(void)
{
	int status;
	pid_t pid;
start:
	pid = process_wait(current, -1, &status, WNOHANG);
	if (pid == NULL || pid == -EAGAIN)
		goto start;
	goto start;
}
