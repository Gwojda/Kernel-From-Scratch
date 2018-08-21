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
	INIT_LIST_HEAD(&pros->map_memory);
	INIT_LIST_HEAD(&pros->signal.sig_queue.list);
	bzero(pros->signal.sig_handler, sizeof(pros->signal.sig_handler));
	return (pros);
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
