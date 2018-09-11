#include "process.h"
#include "errno.h"

int		process_alloc_pid(pid_t *pid)
{
	static pid_t last_pid = PROC_MIN_PID;

	struct list_head *l;
	struct process *p;
	int try = 0;

start:
	*pid = last_pid++;
	if (try++ > PROC_MAX_PID - PROC_MIN_PID)
		return -EAGAIN;
	if (last_pid >= PROC_MAX_PID)
		last_pid = PROC_MIN_PID;
	list_for_each(l, &process_list)
	{
		p = list_entry(l, struct process, plist);
		if (*pid == p->pid)
			goto start;
	}
	return 0;
}

struct process	*process_new()
{
	struct process *proc;

	if ((proc = kmalloc(sizeof(*proc))) == NULL)
		return (NULL);
	bzero(proc, sizeof(*proc));
	INIT_LIST_HEAD(&proc->plist);
	INIT_LIST_HEAD(&proc->children);
	INIT_LIST_HEAD(&proc->mm_heap);
	INIT_LIST_HEAD(&proc->mm_stack);
	INIT_LIST_HEAD(&proc->signal.sig_queue.list);
	bzero(proc->signal.sig_handler, sizeof(proc->signal.sig_handler));
	return (proc);
}
