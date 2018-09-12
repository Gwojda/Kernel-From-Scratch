#include "process.h"
#include "printk.h"
#include "errno.h"
#include "idt.h"
#include "page.h"

void		process_free_all_memory(struct process *proc)
{
	struct map_memory	*pm;
	struct list_head	*l;
	struct list_head	*n;

	list_for_each_safe(l, n, &proc->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (proc == current)
			page_unmap_at(pm->v_addr, pm->flags, pm->size);
		free_phys_block(pm->p_addr, pm->size);
		list_del(l);
		kfree(l);
	}
	list_for_each_safe(l, n, &proc->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (proc == current)
			page_unmap_at(pm->v_addr, pm->flags, pm->size);
		free_phys_block(pm->p_addr, pm->size);
		list_del(l);
		kfree(l);
	}
	if (proc == current)
		page_unmap_at(proc->mm_code.v_addr, proc->mm_code.flags, proc->mm_code.size);
	free_phys_block(proc->mm_code.p_addr, proc->mm_code.size);
}

void		process_die_safe(struct process *proc)
{
	struct list_head	*l;
	struct list_head	*n;

	process_free_all_memory(proc);
	list_for_each_safe(l, n, &proc->signal.sig_queue.list)
		list_del(l);
	if (proc->father)
	{
		add_signal(SIGCHLD, proc->father, SIG_SOFT);
		if (proc->father->signal.sig_handler[SIGCHLD] != NULL)
			goto free_proc;
		proc->state = ZOMBIE;
	}
	else
free_proc:
	{
		proc->state = ZOMBIE;
		free_process(proc);
	}
	// problem en cas de multi coeur ici
	if (proc == current)
	{
		current = NULL;
		reload_process(proc_switch_context, NULL, &switch_process);
	}
}

void		process_die(struct process *proc)
{
	if (proc == current)
	{
		reload_process(proc_switch_context, proc, &process_die_safe);
	}
	process_die_safe(proc);
}
