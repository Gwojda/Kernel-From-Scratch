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
	INIT_LIST_HEAD(&pros->map_memory);
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
	list_for_each_safe(l, n, &pros->map_memory)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (page_unmap(pm->v_addr, pm->flags))
		{
			printk("failled to unmap memory in process with pid : %d\n", pros->pid);
			return ;
		}
		//change here if we can have more than one page peer link in process
		free_phys_block(pm->p_addr, 1);
		list_del(l);
	}
	list_for_each_safe(l, n, &pros->signal.sig_queue.list)
		list_del(l);
	
}

void		free_prosses(struct prosses *pros)
{
	struct list_head	*l;
	struct list_head	*n;

	list_del(&pros->plist);
	list_for_each_safe(l, n, &pros->children)
		list_del(l);
//		child is unattach and so has to be attach to process with pid 1
}

int		copy_prosses(struct prosses *pros, struct prosses *neww)
{
	struct list_head	*l;
	struct map_memory	*new_pm;
	struct map_memory	*pm;
	struct children		*new_child;
	struct sig_queue	*sig_queued;
	char			*tmp;

	neww->state = pros->state;
//	GET NEW PID HERE
	neww->pid = pros->pid;

	neww->uid = pros->uid;
	neww->father = pros;
	INIT_LIST_HEAD(&neww->children);
	new_child = (struct children *)kmalloc(sizeof(*new_child));
	new_child->p = neww;
	list_add(&new_child->list, &pros->children);
	neww->regs = pros->regs;
	if ((tmp = vmalloc(4096)) == NULL)
		return -1;
	list_for_each(l, &pros->map_memory)
	{
		pm = list_entry(l, struct map_memory, plist);
		if ((new_pm = kmalloc(sizeof(*pm))) == NULL)
			return -1;
		new_pm->v_addr = pm->v_addr;
		new_pm->flags = pm->flags;
//		size is always 4096 in every link, for now
		memcpy(tmp, pm->v_addr, 4096);
		if (prosses_memory_switch(pros, 0))
			return -2;
		prosses_memory_add(neww, new_pm->v_addr, PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, 0);
		if (page_map(new_pm->p_addr, new_pm->v_addr, new_pm->flags) == 0)
			return 1;
//		size is always 4096 in every link, for now
		memcpy(new_pm->v_addr, tmp, 4096);
		page_unmap(new_pm->v_addr, new_pm->flags);
		if (prosses_memory_switch(pros, 1))
			return -2;
		list_add(&new_pm->plist, &neww->map_memory);
	}
	list_for_each_entry(sig_queued, &pros->signal.sig_queue.list, list)
		add_signal(sig_queued->sig_handled, neww);
	vfree(tmp);
	return 0;
}

struct prosses	*prosses_dup(struct prosses *pros)
{
	struct prosses *neww = prosses_new();

	copy_prosses(pros, neww);
	return neww;
}

//		this function is NOT working, we need more stuff for thread to make it work

int		create_thread(struct prosses *pros, struct prosses *neww)
{
	struct list_head	*l;
	struct map_memory	*new_pm;
	struct map_memory	*pm;

	neww->state = pros->state;
	neww->pid = pros->pid;
	neww->uid = pros->uid;
//	neww->father = pros->father;
//	neww->children 
	neww->regs = pros->regs;

//	thread need a new stack, actualy he share with here father his stack
	list_for_each(l, &pros->map_memory)
	{
		pm = list_entry(l, struct map_memory, plist);
		if ((new_pm = kmalloc(sizeof(*pm))) == NULL)
			return -1;
		new_pm->v_addr = pm->v_addr;
		new_pm->p_addr = pm->p_addr;
		new_pm->flags = pm->flags;
		list_add(&new_pm->plist, &neww->map_memory);
	}

// should i copy the signal for the child ? I think it's bad idea, need to think about it with Nico
	return 0;
}

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

	list_for_each(l, &pros->map_memory)
	{
		pm = list_entry(l, struct map_memory, plist);
		flags = pm->flags;
		if (add == 0)
			flags &= ~PAGE_PRESENT;
		if (page_map(pm->p_addr, pm->v_addr, flags) == 0)
			return 1;
	}
	return 0;
}

int		prosses_memory_add(struct prosses *pros, void *v_addr, unsigned flags, int imediate)
{
	struct map_memory *pm;
	void *phys;

	if ((phys = page_get_phys(v_addr)))
		// TODO check more thing, it can be ok
		return 1;
	if ((flags & PAGE_PRESENT) == 0)
		return 2;
	if ((pm = kmalloc(sizeof(*pm))) == NULL)
		return 3;
	pm->v_addr = v_addr;
	if ((pm->p_addr = get_phys_block(1)) == NULL)
		return 5;
	// TODO bzero prosses memory
	pm->flags = flags;
	if (imediate)
		if (page_map(pm->p_addr, v_addr, flags) == 0)
		{
			kfree(pm);
			printk("fail to add page\n");
			return 4;
		}
	list_add(&pm->plist, &(pros->map_memory));
	printk(" add page %p\n", pm->v_addr);
	return 0;
}

int		prosses_memory_add_size(struct prosses *pros, void *v_addr, unsigned flags, int imediate, size_t size)
{
	size_t nb_page;

	size += (size_t)v_addr - ((size_t)v_addr & PAGE_ADDR);
	v_addr = (void*)((size_t)v_addr & PAGE_ADDR);
	nb_page = size >> 12;
	if ((size_t)(v_addr + size) & PAGE_FLAG)
		nb_page++;
	while (nb_page)
	{
		if (prosses_memory_add(pros, v_addr, flags, imediate))
			return 1;
		v_addr += 1 << 12;
		nb_page--;
	}
	return 0;
}

struct prosses	*prosses_ini_kern(u32 *v_addr, void* function, size_t size)
{
	struct prosses *pros;

	if ((pros = prosses_new()) == NULL)
		return NULL;

	prosses_memory_add_size(pros, v_addr, PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, 1, size);
	memcpy((void*)((size_t)v_addr & PAGE_ADDR), (void*)((size_t)function & PAGE_ADDR), size + ((size_t)v_addr & PAGE_FLAG));

	prosses_memory_add(pros, (void *)0xC0000000 - (1 << 12), PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, 1);

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
