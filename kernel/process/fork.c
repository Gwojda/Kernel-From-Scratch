#include "process.h"
#include "errno.h"

// same as process_dup
pid_t	fork(struct process *proc)
{
	int err = 0;
	struct process *new;

	if ((new = process_new()) == NULL)
		return -ENOMEM;

	if ((err = copy_process(proc, new)) < 0)
	{
		free_process(new);
		return err;
	}

	new->father = proc;
	return new->pid;
}
