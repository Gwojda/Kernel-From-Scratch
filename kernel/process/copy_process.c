#include "process.h"
#include "page.h"
#include "GDT.h"
#include "printk.h"

static int	process_copy_mem_block(struct map_memory *new_pm, struct map_memory *pm)
{
	char			swap[4096];
	char			swap2[4096];
	void			*p_swap_addr;

	new_pm->v_addr = pm->v_addr;
	new_pm->flags = pm->flags;
	new_pm->size = pm->size;
	if ((new_pm->p_addr = get_phys_block(new_pm->size)) == NULL)
		return -1;
	p_swap_addr = page_get_phys(swap);
	p_swap_addr = page_get_phys(swap2);
	for (size_t i = 0; i < pm->size * 4096; i += 4096)
	{
		access_table_with_physical(page_swap, new_pm->p_addr);
		access_table_with_physical(page_swap2, pm->p_addr);
		memcpy(page_swap, page_swap2, 4096);
	}
	page_map(p_swap_addr, swap, pm->flags);
	page_map(p_swap_addr, swap2, pm->flags);
	return 0;
}

int		copy_process(struct process *proc, struct process *neww)
{
	int err;
	struct map_memory	*new_pm;
	struct map_memory	*pm;
	struct list_head	*l;
	struct children		*new_child;
	struct sig_queue	*sig_queued;

	if (process_alloc_pid(&neww->pid) < 0)
		return -1;
	list_for_each(l, &proc->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		if ((new_pm = kmalloc(sizeof(*pm))) == NULL)
			return -1;
		if (process_copy_mem_block(new_pm, pm) < 0)
		{
			kfree(new_pm);
			return -1;
		}
		list_add(&new_pm->plist, &neww->mm_heap);
	}

	list_for_each(l, &proc->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		if ((new_pm = kmalloc(sizeof(*pm))) == NULL)
			return -1;
		if (process_copy_mem_block(new_pm, pm) < 0)
		{
			kfree(new_pm);
			return -1;
		}
		list_add(&new_pm->plist, &neww->mm_stack);
	}

	if (process_copy_mem_block(&neww->mm_code, &proc->mm_code) < 0)
		return -1;

	list_for_each_entry(sig_queued, &proc->signal.sig_queue.list, list)
		add_signal(sig_queued->sig_handled, neww, sig_queued->state);

	if (!(new_child = (struct children *)kmalloc(sizeof(*new_child))))
		return -1;
	new_child->p = neww;
	neww->state = proc->state;
	if ((err = process_alloc_pid(&neww->pid)))
		return err;
	neww->regs = proc->regs;
	neww->uid = proc->uid;

	INIT_LIST_HEAD(&neww->children);

	neww->father = proc;
	neww->end_value = 0;
	neww->waiting_pid = 0;

	list_add(&new_child->list, &proc->children);
	list_add(&neww->plist, &proc->plist);
	return 0;
}

struct process	*process_dup(struct process *proc)
{
	struct process *neww = process_new();

	if (neww == NULL)
		return NULL;
	if (copy_process(proc, neww) < 0)
	{
		free_process(neww);
		return NULL;
	}
	return neww;
}

//		this function is NOT working, we need more stuff for thread to make it work
/*
int		create_thread(struct process *proc, struct process *neww)
{
	struct map_memory	*new_pm;
	struct map_memory	*pm;
	struct list_head	*l;
	struct children		*new_child;
	struct sig_queue	*sig_queued;

	neww->state = proc->state;
	neww->pid = proc->pid;
	neww->regs = proc->regs;
	neww->uid = proc->uid;

	INIT_LIST_HEAD(&neww->children);

	neww->father = proc;
	new_child = (struct children *)kmalloc(sizeof(*new_child));
	new_child->p = neww;
	list_add(&new_child->list, &proc->children);

	list_for_each(l, &proc->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		if ((new_pm = kmalloc(sizeof(*pm))) == NULL)
			return -1;
		new_pm->v_addr = pm->v_addr;
		new_pm->p_addr = pm->p_addr;
		new_pm->flags = pm->flags;
		new_pm->size = pm->size;
		list_add(&new_pm->plist, &neww->map_memory);
	}

	// dont work here, we need to get a new stack
	list_for_each(l, &proc->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		if ((new_pm = kmalloc(sizeof(*pm))) == NULL)
			return -1;
		new_pm->v_addr = pm->v_addr;
		new_pm->p_addr = pm->p_addr;
		new_pm->flags = pm->flags;
		new_pm->size = pm->size;
		list_add(&new_pm->plist, &neww->mm_stack);
	}

	copy_memory_from_process(&proc->mm_code, &neww->mm_code);

	list_for_each_entry(sig_queued, &proc->signal.sig_queue.list, list)
		add_signal(sig_queued->sig_handled, neww);
	return 0;
}
*/
