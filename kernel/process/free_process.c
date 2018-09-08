#include "process.h"
#include "page.h"
#include "GDT.h"

void		free_process(struct process *proc)
{
	struct list_head	*l;
	struct list_head	*n;
	struct children		*c;

	list_del(&proc->plist);
	if (proc->father)
	{
		list_for_each_entry(c, &proc->father->children, list)
		{
			if (c->p == proc)
			{
				list_del(c);
				kfree(c);
				break ;
			}
		}
	}
	list_for_each_safe(l, n, &proc->children)
	{
		list_del(l);
		kfree(l);
	}
	kfree(proc);
//		child is unattach and so has to be attach to process with pid 1
}
