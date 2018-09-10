#include "process.h"
void sys_restart(void);
void user1(void);
void user1_2(void);
void user2(void);
void user3(void);
void user_shell(void);
void user_hlt(void);
void user_noobcrash(void);
void testwait(void);

void	process_tester(void)
{
//	struct process *p1 = process_ini_kern(user1, (void*)user1 + 0xC0000000, 1 << 12);
//	process_memory_switch(p1, 0);

	//struct process *p0 = process_ini_kern(user2, (void*)user2 + 0xC0000000, 1 << 12);
	//process_memory_switch(p0, 0);
	//p0 = process_dup(p0);
	//process_memory_switch(p0, 0);
	//p0 = process_ini_kern(user_noobcrash, (void*)user_noobcrash + 0xC0000000, 1 << 12);
	//process_memory_switch(p0, 0);


	struct process *p3 = process_ini_kern(testwait, (void*)testwait + 0xC0000000, 1 << 12);
	p3->father = process_get_with_pid(1);
	process_memory_switch(p3, 0);
	for (size_t i = 0; i < 128; ++i)
	{
		pid_t	pid1 = fork(p3);
		add_signal(SIGKILL, process_get_with_pid(pid1), i % 2 ? SIG_SOFT : SIG_HARD);
	}

/*	struct process *p2 = process_ini_kern(user2, (void*)user2 + 0xC0000000, 1 << 12);
	process_memory_switch(p2, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);

	p3 = process_ini_kern(user3, (void*)user3 + 0xC0000000, 1 << 12);
	process_memory_switch(p3, 0);
	pid_t x = fork(p3);
	process_memory_switch(process_get_with_pid(x), 0);
	pid_t y = fork(p3);
	process_memory_switch(process_get_with_pid(y), 0);
	kill(x, 5);*/
}
