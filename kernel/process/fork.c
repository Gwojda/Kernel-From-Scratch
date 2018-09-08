#include "process.h"
#include "errno.h"


// TODO same as process_dup
pid_t	fork(struct process *proc)
{
	int err = 0;
	struct process *new;

	if ((new = process_new()) == NULL)
		return -ENOMEM;

	if ((err = copy_process(proc, new)) < 0)
	{
		free_process(new);// TODO kfree ?
		return err;
	}

	new->father = proc;
	printk("Fork new pid %d\n", new->pid);
	return new->pid;
}
