#include "process.h"
#include "page.h"
#include "GDT.h"

void		free_process(struct process *proc)
{
	struct list_head	*l;
	struct list_head	*n;

	list_del(&proc->plist);
	list_for_each_safe(l, n, &proc->children)
	{
		list_del(l);
		kfree(l);
	}
	kfree(proc);
//		child is unattach and so has to be attach to process with pid 1
}
