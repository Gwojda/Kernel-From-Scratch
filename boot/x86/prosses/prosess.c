#include "prosses.h"
#include "page.h"
#include "GDT.h"
#include "printk.h"

struct prosses *current = NULL;
struct list_head prosses_list = LIST_HEAD_INIT(prosses_list);

struct prosses *prosses_new()
{
	struct prosses *pros;

	if ((pros = kmalloc(sizeof(*pros))) == NULL)
		return (NULL);
	bzero(pros, sizeof(*pros));
	INIT_LIST_HEAD(&pros->children);
	INIT_LIST_HEAD(&pros->mm_heap);
	INIT_LIST_HEAD(&pros->mm_stack);
	INIT_LIST_HEAD(&pros->signal.sig_queue.list);
	bzero(pros->signal.sig_handler, sizeof(pros->signal.sig_handler));
	return (pros);
}

void		prosses_die(struct prosses *pros)
{
	struct map_memory	*pm;
	struct list_head	*l;
	struct list_head	*n;

	if (pros->father)
	{
		add_signal(SIGCHLD, pros->father);
		pros->state = ZOMBIE;
	}
	list_for_each_safe(l, n, &pros->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (page_unmap(pm->v_addr, pm->flags))
		{
			printk("failled to unmap memory in process with pid : %d\n", pros->pid);
			return ;
		}
		free_phys_block(pm->p_addr, pm->size);
		list_del(l);
	}
	list_for_each_safe(l, n, &pros->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (page_unmap(pm->v_addr, pm->flags))
		{
			printk("failled to unmap memory in process with pid : %d\n", pros->pid);
			return ;
		}
		free_phys_block(pm->p_addr, pm->size);
		list_del(l);
	}
	if (page_unmap(pros->mm_code.v_addr, pros->mm_code.flags))
	{
		printk("failled to unmap memory in process with pid : %d\n", pros->pid);
		return ;
	}
	free_phys_block(pros->mm_code.p_addr, pros->mm_code.size);
	list_for_each_safe(l, n, &pros->signal.sig_queue.list)
		list_del(l);
	
}

void		free_prosses(struct prosses *pros)
{
	struct list_head	*l;
	struct list_head	*n;

	list_del(&pros->plist);
	list_for_each_safe(l, n, &pros->children)
	{
		list_del(l);
		kfree(l);
	}
//		child is unattach and so has to be attach to process with pid 1
}

// we should clear this function, she's too big

static int	prosses_copy_mem_block(struct prosses *pros, struct prosses *neww,
		struct map_memory *new_pm, struct map_memory *pm, unsigned pflags)
{
	char			*tmp;

	new_pm->v_addr = pm->v_addr;
	new_pm->flags = pm->flags;
	new_pm->size = pm->size;
	if ((tmp = vmalloc(pm->size * 4096)) == NULL)
		return -1;
	memcpy(tmp, pm->v_addr, pm->size * 4096);
	if (prosses_memory_switch(pros, 0))
	{
		vfree(tmp);
		return -1;
	}
	prosses_memory_add(neww, new_pm->size, new_pm->v_addr, PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, pflags);
	if (page_map(new_pm->p_addr, new_pm->v_addr, new_pm->flags) == 0)
	{
		free_phys_block(new_pm->p_addr, new_pm->size);
		vfree(tmp);
		return -2;
	}
	memcpy(new_pm->v_addr, tmp, new_pm->size * 4096);
	page_unmap(new_pm->v_addr, new_pm->flags);
	vfree(tmp);
	if (prosses_memory_switch(pros, 1))
		return -1;
	return 0;
}


int		copy_prosses(struct prosses *pros, struct prosses *neww)
{
	struct map_memory	*new_pm;
	struct map_memory	*pm;
	struct list_head	*l;
	struct children		*new_child;
	struct sig_queue	*sig_queued;
	int			checker;

	neww->state = pros->state;
//	GET NEW PID HERE
	neww->pid = pros->pid;
	neww->regs = pros->regs;
	neww->uid = pros->uid;

	INIT_LIST_HEAD(&neww->children);

	neww->father = pros;
	new_child = (struct children *)kmalloc(sizeof(*new_child));
	new_child->p = neww;
	list_add(&new_child->list, &pros->children);

	list_for_each(l, &pros->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		if ((new_pm = kmalloc(sizeof(*pm))) == NULL)
			return -1;
		checker = prosses_copy_mem_block(pros, neww, new_pm, pm, PROC_MEM_ADD_STACK);
		if (checker == -1)
			return -1;
		else if (checker == -2)
		{
			kfree(new_pm);
			return -1;
		}
		list_add(&new_pm->plist, &neww->mm_heap);
	}

	list_for_each(l, &pros->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		if ((new_pm = kmalloc(sizeof(*pm))) == NULL)
			return -1;
		checker = prosses_copy_mem_block(pros, neww, new_pm, pm, PROC_MEM_ADD_HEAP);
		if (checker == -1)
			return -1;
		else if (checker == -2)
		{
			kfree(new_pm);
			return -1;
		}
		list_add(&new_pm->plist, &neww->mm_stack);
	}

	prosses_copy_mem_block(pros, neww, &neww->mm_code, &pros->mm_code, PROC_MEM_ADD_CODE);

	list_for_each_entry(sig_queued, &pros->signal.sig_queue.list, list)
		add_signal(sig_queued->sig_handled, neww);
	return 0;
}

struct prosses	*prosses_dup(struct prosses *pros)
{
	struct prosses *neww = prosses_new();

	copy_prosses(pros, neww);
	return neww;
}

//		this function is NOT working, we need more stuff for thread to make it work
/*
int		create_thread(struct prosses *pros, struct prosses *neww)
{
	struct map_memory	*new_pm;
	struct map_memory	*pm;
	struct list_head	*l;
	struct children		*new_child;
	struct sig_queue	*sig_queued;

	neww->state = pros->state;
	neww->pid = pros->pid;
	neww->regs = pros->regs;
	neww->uid = pros->uid;

	INIT_LIST_HEAD(&neww->children);

	neww->father = pros;
	new_child = (struct children *)kmalloc(sizeof(*new_child));
	new_child->p = neww;
	list_add(&new_child->list, &pros->children);

	list_for_each(l, &pros->mm_heap)
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
	list_for_each(l, &pros->mm_stack)
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

	copy_memory_from_prosses(&pros->mm_code, &neww->mm_code);

	list_for_each_entry(sig_queued, &pros->signal.sig_queue.list, list)
		add_signal(sig_queued->sig_handled, neww);
	return 0;
}
*/

static int	end_of_child(struct prosses *child, struct prosses *father)
{
	int	ret = child->end_value;
	free_prosses(child);
	father->state = RUN;
	return ret;
}

int		child_ended(struct prosses *pros)
{
	struct prosses	*p;

	list_for_each_entry(p, &pros->children, children)
	{
		if (p->pid == pros->waiting_pid)
			return end_of_child(p, pros);
	}
	return -1;
}

int		process_wait(struct prosses *pros, pid_t waiting_on_pid)
{
	struct prosses	*p;

	list_for_each_entry(p, &pros->children, children)
	{
		if (p->pid == waiting_on_pid)
			goto start_waiting;
	}
	goto err;

start_waiting:
	if (p->state == ZOMBIE)
		return end_of_child(p, pros);
	else
	{
		pros->state = STOPPED;
		pros->signal.sig_handler[SIGCHLD] = child_ended;
		pros->waiting_pid = waiting_on_pid;
	}
	return (0);

err:
	return (-1);
}

int		getuid(struct prosses *pros)
{
	return pros->uid;
}

int		prosses_memory_switch(struct prosses *pros, int add)
{
	struct map_memory *pm;
	struct list_head *l;
	unsigned flags;

	list_for_each(l, &pros->mm_heap)
	{
		pm = list_entry(l, struct map_memory, plist);
		flags = pm->flags;
		if (add == 0)
			flags &= ~PAGE_PRESENT;
		if (page_map_range(pm->p_addr, pm->v_addr, flags, pm->size) == 0)
			return 1;
	}
	list_for_each(l, &pros->mm_stack)
	{
		pm = list_entry(l, struct map_memory, plist);
		flags = pm->flags;
		if (add == 0)
			flags &= ~PAGE_PRESENT;
		if (page_map_range(pm->p_addr, pm->v_addr, flags, pm->size) == 0)
			return 1;
	}
	flags = pros->mm_code.flags;
	if (add == 0)
		flags &= ~PAGE_PRESENT;
	if (page_map_range(pros->mm_code.p_addr, pros->mm_code.v_addr, flags, pros->mm_code.size) == 0)
		return 1;
	return 0;
}

// -------------------------------------------------------------------------------------
// changement on proc has to be done here

int		prosses_memory_add(struct prosses *pros, size_t size, void *v_addr, unsigned mflags, unsigned pflags)
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
	// TODO bzero prosses memory
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
			list_add(&pm->plist, &(pros->mm_heap));
			goto end;
			break ;
		case PROC_MEM_ADD_STACK:
			list_add(&pm->plist, &(pros->mm_stack));
			goto end;
			break ;
		case PROC_MEM_ADD_CODE:
			pros->mm_code = *pm;
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

struct prosses	*prosses_ini_kern(u32 *v_addr, void* function, size_t size)
{
	struct prosses *pros;

	if ((pros = prosses_new()) == NULL)
		return NULL;

	prosses_memory_add(pros, size, v_addr, PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, PROC_MEM_ADD_IMEDIATE | PROC_MEM_ADD_CODE);
	memcpy((void*)((size_t)v_addr & PAGE_ADDR), (void*)((size_t)function & PAGE_ADDR), size + ((size_t)v_addr & PAGE_FLAG));
	prosses_memory_add(pros, 1, (void *)0xC0000000 - (1 << 12), PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, PROC_MEM_ADD_IMEDIATE | PROC_MEM_ADD_STACK);

	pros->regs.eax = 0;
	pros->regs.ecx = 0;
	pros->regs.edx = 0;
	pros->regs.ebx = 0;

	pros->regs.esp = 0xC0000000 - 0x10;/// - (1 << 12) / 2;

	pros->regs.ebp = 0;
	pros->regs.esi = 0;
	pros->regs.edi = 0;

	pros->regs.eip = (u32)v_addr;
	pros->regs.eflags = 0;

	pros->regs.cs = GDT_SEG_KCODE;
	pros->regs.ss = GDT_SEG_KSTACK;
	pros->regs.ds = GDT_SEG_KDATA;
	pros->regs.es = GDT_SEG_KDATA;
	pros->regs.fs = GDT_SEG_KDATA;
	pros->regs.gs = GDT_SEG_KDATA;
	//printk("   %p %p %p %p\n", &prosses, &prosses.next, &(prosses.next)->next, &((prosses.next)->next)->next);
	list_add(&pros->plist, &prosses_list);
	//printk("init ok %p %p %p %p\n", &prosses, &prosses.next, &(prosses.next)->next, &((prosses.next)->next)->next);
	return pros;
}
