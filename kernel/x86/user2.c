#include "syscall.h"
#include "shell.h"
#include "process.h"
#include "errno.h"
#include "stream.h"

void sys(void);
extern struct stream stream_test;

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
	asm("cli");
	pid = process_waitpid(current, -1, &status, WNOHANG);
	asm("sti");
	if (pid == -ECHILD)
		exit(current, 4);
	if (pid == 0 || pid == -EAGAIN)
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

__attribute__ ((section(".ucode"))) void user_piperead(void)
{
	char c;
	int ret;
start:
	if ((ret = stream_read(&stream_test, &c, 1)) == -1)
		exit(current, -1);
	if (ret == 1)
		printk("%c", c);
	goto start;
}
__attribute__ ((section(".ucode"))) void user_pipewrite(void)
{
	char *str = "Hello,\nit's look stream work\n";
	size_t basse = 0;
	int ret;
	while ((ret = stream_write(&stream_test, str + basse, strlen(str) - basse)) != -1)
	{
		if (basse >= strlen(str))
			break;
		basse += ret;
	}
	exit(current, 0);
}

__attribute__ ((section(".ucode"))) void init(void)
{
	int status;
	pid_t pid;
start:
	asm("cli");
	pid = process_waitpid(current, -1, &status, WNOHANG);
	asm("sti");
	if (pid == 0 || pid == -EAGAIN)
		goto start;
	goto start;
}
