#include "process.h"
#include "page.h"
#include "GDT.h"
#include "printk.h"

static int	end_of_child(struct process *child, struct process *father)
{
	int	ret = child->end_value;
	free_process(child);
	father->state = RUN;
	return ret;
}

int		child_ended(struct process *proc)
{
	struct process	*p;

	list_for_each_entry(p, &proc->children, children)
	{
		if (p->pid == proc->waiting_pid)
			return end_of_child(p, proc);
	}
	return -1;
}

int		process_wait(struct process *proc, pid_t waiting_on_pid)
{
	struct process	*p;

	list_for_each_entry(p, &proc->children, children)
	{
		if (p->pid == waiting_on_pid)
			goto start_waiting;
	}
	goto err;

start_waiting:
	if (p->state == ZOMBIE)
		return end_of_child(p, proc);
	else
	{
		proc->state = STOPPED;
		proc->signal.sig_handler[SIGCHLD] = child_ended;
		proc->waiting_pid = waiting_on_pid;
	}
	return (0);

err:
	return (-1);
}
