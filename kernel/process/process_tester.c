#include "process.h"
#include "syscall.h"
#include "stream.h"

void sys_restart(void);
void user1(void);
void user1_2(void);
void user2(void);
void user3(void);
void user_shell(void);
void user_hlt(void);
void user_noobcrash(void);
void testwait(void);
void user_piperead(void);
void user_pipewrite(void);

struct stream stream_test;

void	process_tester(void)
{
	struct process *p3;

	struct process *p0 = process_ini_kern(user2, (void*)user2 + 0xC0000000, 1 << 12);
	process_memory_switch(p0, 0);
	p0 = process_dup(p0);
	process_memory_switch(p0, 0);

	p0 = process_ini_kern(user_noobcrash, (void*)user_noobcrash + 0xC0000000, 1 << 12);
	process_memory_switch(p0, 0);

	p3 = process_ini_kern(testwait, (void*)testwait + 0xC0000000, 1 << 12);
	p3->father = process_get_with_pid(1);
	process_memory_switch(p3, 0);
	for (size_t i = 0; i < 128; ++i)
	{
		pid_t	pid1 = fork(p3);
		add_signal(SIGKILL, process_get_with_pid(pid1), i % 2 ? SIG_SOFT : SIG_HARD);
	}

//	scheduler test

	for (size_t i = 0; i < 2048; ++i)
	{
		p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);

		if (!p3)
		{
			printk("i = %d\n", i);
			break ;
		}
		else
			process_memory_switch(p3, 0);
	}

//	fork test

	p3 = process_ini_kern(testwait, (void*)testwait + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	pid_t x = fork(p3);
	process_memory_switch(process_get_with_pid(x), 0);
	pid_t y = fork(p3);
	process_memory_switch(process_get_with_pid(y), 0);
	p3->uid = 100000;
	printk("kill %d\n", kill(p3, x, SIGKILL));

	struct process *ppipe = process_ini_kern(user_piperead, (void*)user_piperead + 0xC0000000, 1 << 12);
	process_memory_switch(ppipe, 0);
	ppipe = process_ini_kern(user_pipewrite, (void*)user_pipewrite + 0xC0000000, 1 << 12);
	process_memory_switch(ppipe, 0);
}
