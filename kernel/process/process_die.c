#include "process.h"
#include "printk.h"
#include "errno.h"

static void	free_page_map_memory(struct process *proc, struct map_memory *pm, struct list_head *l)
{
	if (proc == current)
		page_unmap(pm->v_addr, pm->flags);
	free_phys_block(pm->p_addr, 1);
	if (pm->size == 1)
		list_del(l);
	else
	{
		pm->size--;
		pm->v_addr += 4096;
		pm->p_addr += 4096;
	}
}

//used to free one virtual page from process
void		process_free_memory_page(struct process *proc, void *vaddr)
{
	struct map_memory	*pm;
	struct list_head	*l;
	struct list_head	*n;

	list_for_each_safe(l, n, &proc->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (pm->v_addr != vaddr)
			continue ;
		free_page_map_memory(proc, pm, l);
		return ;
	}
	list_for_each_safe(l, n, &proc->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (pm->v_addr != vaddr)
			continue ;
		free_page_map_memory(proc, pm, l);
		return ;
	}
	if (proc->mm_code.v_addr != vaddr)
		return ;
	if (proc == current)
		page_unmap(proc->mm_code.v_addr, proc->mm_code.flags);
	free_phys_block(proc->mm_code.p_addr, 1);
	if (proc->mm_code.size == 1)
	{
		proc->mm_code.size = 0;
		proc->mm_code.v_addr = NULL;
		proc->mm_code.p_addr = NULL;
	}
	else
	{
		proc->mm_code.size--;
		proc->mm_code.v_addr += 4096;
		proc->mm_code.p_addr += 4096;
	}
}

static void	free_map_memory(struct process *proc, struct map_memory *pm, struct list_head *l)
{
	if (proc == current)
		page_unmap(pm->v_addr, pm->flags);
	free_phys_block(pm->p_addr, pm->size);
	list_del(l);
}

int		process_free_memory(struct process *proc, void *vaddr, size_t size)
{
	struct map_memory	*pm;
	struct list_head	*l;
	struct list_head	*n;

	list_for_each_safe(l, n, &proc->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (pm->v_addr != vaddr)
			continue ;
		if (pm->size != size)
			return -EINVAL;
		free_map_memory(proc, pm, l);
	}
	list_for_each_safe(l, n, &proc->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (pm->v_addr != vaddr)
			continue ;
		if (pm->size != size)
			return -EINVAL;
		free_map_memory(proc, pm, l);
	}
	if (pm->size != size ||
	proc->mm_code.v_addr != vaddr)
		return -EINVAL;
	if (proc == current)
		page_unmap(proc->mm_code.v_addr, proc->mm_code.flags);
	free_phys_block(proc->mm_code.p_addr, proc->mm_code.size);
	proc->mm_code.size = 0;
	proc->mm_code.v_addr = NULL;
	proc->mm_code.p_addr = NULL;
	return 0;
}

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
	}
	list_for_each_safe(l, n, &proc->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (proc == current)
			page_unmap_at(pm->v_addr, pm->flags, pm->size);
		free_phys_block(pm->p_addr, pm->size);
		list_del(l);
	}
	if (proc == current)
		page_unmap_at(proc->mm_code.v_addr, proc->mm_code.flags, proc->mm_code.size);
	free_phys_block(proc->mm_code.p_addr, proc->mm_code.size);
}

void		process_die(struct process *proc)
{
	struct list_head	*l;
	struct list_head	*n;

	process_free_all_memory(proc);
	list_for_each_safe(l, n, &proc->signal.sig_queue.list)
		list_del(l);
	if (proc->father)
	{
		add_signal(SIGCHLD, proc->father, SIG_SOFT);
		proc->state = ZOMBIE;
	}
	else
		proc->state = ZOMBIE;
}
