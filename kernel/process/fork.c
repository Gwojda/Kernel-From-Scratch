#include "process.h"
#include "errno.h"


// TODO same as process_dup
pid_t	fork(struct process *proc)
{
	int err = 0;
	struct process *new;

	if ((new = kmalloc(sizeof(*new))) == NULL)
		return -ENOMEM;

	if ((err = copy_process(proc, new)) != 0)
	{
		free_process(new);
		return err;
	}

	// TODO return
	return new->pid;
}
