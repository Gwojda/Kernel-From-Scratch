#include "process.h"
#include "printk.h"

// TODO die a not maped process
// if proc == current
void		process_die(struct process *proc)
{
	struct map_memory	*pm;
	struct list_head	*l;
	struct list_head	*n;

	list_for_each_safe(l, n, &proc->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (page_unmap(pm->v_addr, pm->flags))
		{
			printk("failled to unmap memory in process with pid : %d\n", proc->pid);
		}
		free_phys_block(pm->p_addr, pm->size);
		list_del(l);
	}
	list_for_each_safe(l, n, &proc->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (page_unmap(pm->v_addr, pm->flags))
		{
			printk("failled to unmap memory in process with pid : %d\n", proc->pid);
		}
		free_phys_block(pm->p_addr, pm->size);
		list_del(l);
	}
	if (page_unmap(proc->mm_code.v_addr, proc->mm_code.flags))
	{
		printk("failled to unmap memory in process with pid : %d\n", proc->pid);
	}
	free_phys_block(proc->mm_code.p_addr, proc->mm_code.size);
	list_for_each_safe(l, n, &proc->signal.sig_queue.list)
		list_del(l);
	if (proc->father)
	{
		add_signal(SIGCHLD, proc->father);
		proc->state = ZOMBIE;
	}
	else
		proc->state = ZOMBIE;
	// TODO else -> remove the process from the list
	// kernel panic ?
}
