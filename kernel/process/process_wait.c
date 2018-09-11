#include "process.h"
#include "page.h"
#include "GDT.h"
#include "printk.h"
#include "errno.h"

static int	end_of_child(struct process *child, struct process *father)
{
	father->waiting_return = child->end_value;
	father->waiting_pid = child->pid;
	free_process(child);
	father->state = RUN;
	return 0;
}

int		child_ended(struct process *proc, int signum)
{
	(void)signum;
	struct children *pc;
	struct process	*p;

	list_for_each_entry(pc, &proc->children, list)
	{
		p = pc->p;
		if (proc->waiting_pid == -1 && p->state == ZOMBIE)
		{
			proc->waiting_pid = p->pid;
			return end_of_child(p, proc);
		}
		if (p->pid == proc->waiting_pid && p->state == ZOMBIE)
			return end_of_child(p, proc);
	}
	return -1;
}

pid_t		process_waitpid(struct process *proc, pid_t waiting_on_pid, int *wstatus, int option)
{
	int err = 0;
	struct children *pc;
	struct process *p;

	if (waiting_on_pid == 0 || waiting_on_pid < -1)
	{
		err = -EINVAL;
		goto end;
	}

	if ((proc->waiting_pid == waiting_on_pid || waiting_on_pid == -1) && proc->waiting_return)
	{
		*wstatus = proc->waiting_return;
		proc->waiting_return = 0;
		return proc->waiting_pid;
	}

	list_for_each_entry(pc, &proc->children, list)
	{
		p = pc->p;
		if (waiting_on_pid == -1 && p->state == ZOMBIE)
			goto start_waiting;
		if (p->pid == waiting_on_pid)
			goto start_waiting;
	}

	if (waiting_on_pid <= 0 && proc->children.next != &(proc->children))
	{
		if (option & WNOHANG)
			return (0);
		goto wait_signal;
	}
	err = -ECHILD;
	goto end;

start_waiting:
	if (p->state == ZOMBIE)
	{
		end_of_child(p, proc);
		*wstatus = proc->waiting_return;
		proc->waiting_return = 0;
		return proc->waiting_pid;
	}
	else if (!(option & WNOHANG))
	{
wait_signal:
		printk("stopped\n");
		proc->state = STOPPED;
		proc->waiting_pid = waiting_on_pid;
		err = -EWOULDBLOCK;
	}
end:
	return (err);
}

pid_t	process_wait(struct process *proc, int *status)
{
	return (process_waitpid(proc, -1, status, 0));
}
