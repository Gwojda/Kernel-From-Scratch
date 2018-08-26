#include "process.h"
#include "page.h"
#include "GDT.h"
#include "printk.h"

struct process *current = NULL;
struct list_head process_list = LIST_HEAD_INIT(process_list);

struct process *process_new()
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

void		process_die(struct process *proc)
{
	struct map_memory	*pm;
	struct list_head	*l;
	struct list_head	*n;

	if (proc->father)
	{
		add_signal(SIGCHLD, proc->father);
		proc->state = ZOMBIE;
	}
	list_for_each_safe(l, n, &proc->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (page_unmap(pm->v_addr, pm->flags))
		{
			printk("failled to unmap memory in process with pid : %d\n", proc->pid);
			return ;
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
			return ;
		}
		free_phys_block(pm->p_addr, pm->size);
		list_del(l);
	}
	if (page_unmap(proc->mm_code.v_addr, proc->mm_code.flags))
	{
		printk("failled to unmap memory in process with pid : %d\n", proc->pid);
		return ;
	}
	free_phys_block(proc->mm_code.p_addr, proc->mm_code.size);
	list_for_each_safe(l, n, &proc->signal.sig_queue.list)
		list_del(l);
	
}

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
//		child is unattach and so has to be attach to process with pid 1
}

// we should clear this function, she's too big

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
	if (page_map(new_pm->p_addr, new_pm->v_addr, new_pm->flags) == 0)
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

static int	end_of_child(struct process *child, struct process *father)
{
	int	ret = child->end_value;
	free_process(child);
	father->state = RUN;
	return ret;
}

int		child_ended(struct process *proc)
{
	struct process	*p;

	list_for_each_entry(p, &proc->children, children)
	{
		if (p->pid == proc->waiting_pid)
			return end_of_child(p, proc);
	}
	return -1;
}

int		process_wait(struct process *proc, pid_t waiting_on_pid)
{
	struct process	*p;

	list_for_each_entry(p, &proc->children, children)
	{
		if (p->pid == waiting_on_pid)
			goto start_waiting;
	}
	goto err;

start_waiting:
	if (p->state == ZOMBIE)
		return end_of_child(p, proc);
	else
	{
		proc->state = STOPPED;
		proc->signal.sig_handler[SIGCHLD] = child_ended;
		proc->waiting_pid = waiting_on_pid;
	}
	return (0);

err:
	return (-1);
}

int		getuid(struct process *proc)
{
	return proc->uid;
}

int		process_memory_switch(struct process *proc, int add)
{
	struct map_memory *pm;
	struct list_head *l;
	unsigned flags;

	list_for_each(l, &proc->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		flags = pm->flags;
		if (add == 0)
			flags &= ~PAGE_PRESENT;
		if (page_map_range(pm->p_addr, pm->v_addr, flags, pm->size) == 0)
			return 1;
	}
	list_for_each(l, &proc->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		flags = pm->flags;
		if (add == 0)
			flags &= ~PAGE_PRESENT;
		if (page_map_range(pm->p_addr, pm->v_addr, flags, pm->size) == 0)
			return 1;
	}
	flags = proc->mm_code.flags;
	if (add == 0)
		flags &= ~PAGE_PRESENT;
	if (page_map_range(proc->mm_code.p_addr, proc->mm_code.v_addr, flags, proc->mm_code.size) == 0)
		return 1;
	return 0;
}

// -------------------------------------------------------------------------------------
// changement on proc has to be done here

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
		return -1;
	if ((mflags & PAGE_PRESENT) == 0)
		return -2;
	if ((pm = kmalloc(sizeof(*pm))) == NULL)
		return -3;
	pm->v_addr = v_addr;
	if ((pm->p_addr = get_phys_block(size)) == NULL)
	{
		ret = -4;
		goto err;
	}
	// TODO bzero process memory
	pm->flags = mflags;
	pm->size = size;
	if (pflags & PROC_MEM_ADD_IMEDIATE)
	{
		if (page_map_range(pm->p_addr, v_addr, mflags, size) == 0)
		{
			ret = -4;
			goto err2;
		}
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
	printk(" virt addr %p, phys addr = %p, size = %d\n", pm->v_addr, pm->p_addr, pm->size * 4096);
	return ret;

}

struct process	*process_ini_kern(u32 *v_addr, void* function, size_t size)
{
	struct process *proc;

	if ((proc = process_new()) == NULL)
		return NULL;

	process_memory_add(proc, size, v_addr, PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, PROC_MEM_ADD_IMEDIATE | PROC_MEM_ADD_CODE);
	memcpy((void*)((size_t)v_addr & PAGE_ADDR), (void*)((size_t)function & PAGE_ADDR), size + ((size_t)v_addr & PAGE_FLAG));
	process_memory_add(proc, 1, (void *)0xC0000000 - (1 << 12), PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, PROC_MEM_ADD_IMEDIATE | PROC_MEM_ADD_STACK);

	proc->regs.eax = 0;
	proc->regs.ecx = 0;
	proc->regs.edx = 0;
	proc->regs.ebx = 0;

	proc->regs.esp = 0xC0000000 - 0x10;/// - (1 << 12) / 2;

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
	//printk("   %p %p %p %p\n", &process, &process.next, &(process.next)->next, &((process.next)->next)->next);
	list_add(&proc->plist, &process_list);
	//printk("init ok %p %p %p %p\n", &process, &process.next, &(process.next)->next, &((process.next)->next)->next);
	return proc;
}
