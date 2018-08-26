#include "process.h"
#include "page.h"
#include "GDT.h"
#include "printk.h"

static int	process_copy_mem_block(struct process *proc, struct process *neww,
		struct map_memory *new_pm, struct map_memory *pm, unsigned pflags)
{
	char			*tmp;

	new_pm->v_addr = pm->v_addr;
	new_pm->flags = pm->flags;
	new_pm->size = pm->size;
	if ((tmp = vmalloc(pm->size * 4096)) == NULL)
		return -1;
	memcpy(tmp, pm->v_addr, pm->size * 4096);
	if (process_memory_switch(proc, 0))
	{
		vfree(tmp);
		return -1;
	}
	process_memory_add(neww, new_pm->size, new_pm->v_addr, PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, pflags);
	if (page_map(new_pm->p_addr, new_pm->v_addr, new_pm->flags))
	{
		free_phys_block(new_pm->p_addr, new_pm->size);
		vfree(tmp);
		return -2;
	}
	memcpy(new_pm->v_addr, tmp, new_pm->size * 4096);
	page_unmap(new_pm->v_addr, new_pm->flags);
	vfree(tmp);
	if (process_memory_switch(proc, 1))
		return -1;
	return 0;
}

int		copy_process(struct process *proc, struct process *neww)
{
	struct map_memory	*new_pm;
	struct map_memory	*pm;
	struct list_head	*l;
	struct children		*new_child;
	struct sig_queue	*sig_queued;
	int			checker;

	neww->state = proc->state;
//	GET NEW PID HERE
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
		checker = process_copy_mem_block(proc, neww, new_pm, pm, PROC_MEM_ADD_STACK);
		if (checker == -1)
			return -1;
		else if (checker == -2)
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
		checker = process_copy_mem_block(proc, neww, new_pm, pm, PROC_MEM_ADD_HEAP);
		if (checker == -1)
			return -1;
		else if (checker == -2)
		{
			kfree(new_pm);
			return -1;
		}
		list_add(&new_pm->plist, &neww->mm_stack);
	}

	process_copy_mem_block(proc, neww, &neww->mm_code, &proc->mm_code, PROC_MEM_ADD_CODE);

	list_for_each_entry(sig_queued, &proc->signal.sig_queue.list, list)
		add_signal(sig_queued->sig_handled, neww);
	return 0;
}

struct process	*process_dup(struct process *proc)
{
	struct process *neww = process_new();

	copy_process(proc, neww);
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
