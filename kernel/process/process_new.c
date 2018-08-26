#include "process.h"

struct process	*process_new()
{
	struct process *proc;

	if ((proc = kmalloc(sizeof(*proc))) == NULL)
		return (NULL);
	bzero(proc, sizeof(*proc));
	INIT_LIST_HEAD(&proc->children);
	INIT_LIST_HEAD(&proc->mm_heap);
	INIT_LIST_HEAD(&proc->mm_stack);
	INIT_LIST_HEAD(&proc->signal.sig_queue.list);
	bzero(proc->signal.sig_handler, sizeof(proc->signal.sig_handler));
	return (proc);
}
