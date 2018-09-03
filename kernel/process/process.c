#include "process.h"
#include "page.h"
#include "GDT.h"
#include "printk.h"
#include "errno.h"

struct process *current = NULL;
struct process *process_hlt = NULL;
struct list_head process_list = LIST_HEAD_INIT(process_list);

int		process_memory_switch(struct process *proc, int add)
{
	int err = 0;
	struct map_memory *pm;
	struct list_head *l;
	unsigned flags;

	list_for_each(l, &proc->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		flags = pm->flags;
		if (add == 0)
			flags &= ~PAGE_PRESENT;
		if ((err = page_map_range(pm->p_addr, pm->v_addr, flags, pm->size)))
			return err;
	}
	list_for_each(l, &proc->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		flags = pm->flags;
		if (add == 0)
			flags &= ~PAGE_PRESENT;
		if ((err = page_map_range(pm->p_addr, pm->v_addr, flags, pm->size)))
			return err;
	}
	flags = proc->mm_code.flags;
	if (add == 0)
		flags &= ~PAGE_PRESENT;
	if ((err = page_map_range(proc->mm_code.p_addr, proc->mm_code.v_addr, flags, proc->mm_code.size)))
		return err;
	return 0;
}

int		process_memory_add(struct process *proc, size_t size, void *v_addr, unsigned mflags, unsigned pflags)
{
	struct map_memory *pm;
	void *phys;
	int ret = 0;

	size += (size_t)v_addr - ((size_t)v_addr & PAGE_ADDR);
	v_addr = (void*)((size_t)v_addr & PAGE_ADDR);
	size = (size % 4096) ? (size >> 12) + 1: size >> 12;
	if ((phys = page_get_phys(v_addr)))
		// TODO check more thing, it can be ok
		return -ENOMEM;
	if ((mflags & PAGE_PRESENT) == 0)
		return -EINVAL;
	if ((pm = kmalloc(sizeof(*pm))) == NULL)
		return -ENOMEM;
	pm->v_addr = v_addr;
	if ((pm->p_addr = get_phys_block(size)) == NULL)
	{
		ret = -ENOMEM;
		goto err;
	}
	pm->flags = mflags;
	pm->size = size;
	phys_bzero(pm->p_addr, pm->flags);
	if (pflags & PROC_MEM_ADD_IMEDIATE)
	{
		if ((ret = page_map_range(pm->p_addr, v_addr, mflags, size)))
			goto err2;
		pflags &= ~(PROC_MEM_ADD_IMEDIATE);
	}
	switch (pflags)
	{
		case PROC_MEM_ADD_HEAP:
			list_add(&pm->plist, &(proc->mm_heap));
			goto end;
			break ;
		case PROC_MEM_ADD_STACK:
			list_add(&pm->plist, &(proc->mm_stack));
			goto end;
			break ;
		case PROC_MEM_ADD_CODE:
			proc->mm_code = *pm;
			goto err;
			break ;
		default:
			break ;
	}
err2:
	free_phys_block(pm->p_addr, pm->size);

err:
	kfree(pm);

end:
//	printk(" virt addr %p, phys addr = %p, size = %d\n", pm->v_addr, pm->p_addr, pm->size * 4096);
	return ret;
}

void sys(void);

struct process	*process_ini_kern(u32 *v_addr, void* function, size_t size)
{
	int err;
	struct process *proc;

	if ((proc = process_new()) == NULL)
		return NULL;
	if ((err = process_alloc_pid(&proc->pid)))
	{
		free_process(proc);
		return NULL;
	}

	if (process_memory_add(proc, size, v_addr, PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, PROC_MEM_ADD_IMEDIATE | PROC_MEM_ADD_CODE))
		// TODO free
		return NULL;
	memcpy((void*)((size_t)v_addr & PAGE_ADDR), (void*)((size_t)function & PAGE_ADDR), size + ((size_t)v_addr & PAGE_FLAG));
	if (process_memory_add(proc, 2 << 12, (void *)0xC0000000 - (2 << 12), PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, PROC_MEM_ADD_IMEDIATE | PROC_MEM_ADD_STACK))
		// TODO free
		return NULL;

	proc->regs.eax = 0;
	proc->regs.ecx = 0;
	proc->regs.edx = 0;
	proc->regs.ebx = 0;

	proc->regs.esp = 0xC0000000 - 4096;/// - (1 << 12) / 2;

	proc->regs.ebp = 0;
	proc->regs.esi = 0;
	proc->regs.edi = 0;

	proc->regs.eip = (u32)v_addr;
	proc->regs.eflags = 0;

	proc->regs.cs = GDT_SEG_KCODE;
	proc->regs.ss = GDT_SEG_KSTACK;
	proc->regs.ds = GDT_SEG_KDATA;
	proc->regs.es = GDT_SEG_KDATA;
	proc->regs.fs = GDT_SEG_KDATA;
	proc->regs.gs = GDT_SEG_KDATA;
	list_add(&proc->plist, &process_list);
	return proc;
}
