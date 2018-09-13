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

__attribute__ ((section(".ucode"))) void testmmap(void)
{
	while (1)
	{
		asm("cli");
		void *p = mmap(current, NULL, 1 << 12, PROT_READ, MAP_ANON, -1, 0);
		printk("p = %p\n", p);
		munmap(current, p, 1 << 12, 0);
		void *p1 = mmap(current, NULL, 1 << 12, PROT_READ, MAP_ANON, -1, 0);
		printk("p = %p\n", p1);
		void *p2 = mmap(current, NULL, 1 << 12, PROT_READ, MAP_ANON, -1, 0);
		printk("p = %p\n", p2);
		void *p3 = mmap(current, NULL, 1 << 12, PROT_READ, MAP_ANON, -1, 0);
		printk("p = %p\n", p3);
		void *p4 = mmap(current, NULL, 1 << 12, PROT_READ, MAP_ANON, -1, 0);
		printk("p = %p\n", p4);
		void *p5 = mmap(current, NULL, 1 << 12, PROT_READ, MAP_ANON, -1, 0);
		printk("p = %p\n", p5);

		munmap(current, p1, 1 << 12, 0);
		munmap(current, p2, 1 << 12, 0);
		munmap(current, p3, 1 << 12, 0);
		munmap(current, p4, 1 << 12, 0);
		munmap(current, p5, 1 << 12, 0);

		char *p6 = mmap(current, NULL, 1 << 12, PROT_WRITE, MAP_ANON, -1, 0);
		for (size_t i = 0; i < 1 << 12; i++)
			p6[i] = 1;

		char *p7 = mmap(current, NULL, 1 << 12, PROT_WRITE, MAP_ANON | MAP_FIXED, -1, 0);
		printk("p FAIL = %p\n", p7);

		char *p8 = mmap(current, p6, 1 << 12, PROT_WRITE, MAP_ANON | MAP_FIXED, -1, 0);
		printk("p FAIL = %p\n", p8);

		char *p9 = mmap(current, (void *)(3 << 12), 1 << 12, PROT_WRITE, MAP_ANON | MAP_FIXED, -1, 0);
		printk("p = %p\n", p9);

		for (size_t i = 0; i < 1 << 12; i++)
			p9[i] = 1;

		munmap(current, p6, 1 << 12, 0);
		munmap(current, p9, 1 << 12, 0);

		asm("sti");

		for (size_t i = 0; i < 0x10000000;i++);
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
