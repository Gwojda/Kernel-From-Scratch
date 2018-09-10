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
	if ((ret = phys_bzero(pm->p_addr, pm->size)))
		goto err2;
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
			ret = -EINVAL;
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

size_t mmap_get_number_of_page(size_t size)
{
	if (size % 4096)
		size += 4096;
	return (size >> 12);
}

void *mmap_get_block_l(struct list_head *b, void *addr)
{
	struct list_head *l;
	struct map_memory *pm;

	list_for_each(l, b)
	{
		pm = list_entry(l, struct map_memory, plist);
		if (pm->v_addr <= addr && (void *)pm->v_addr + (pm->size << 12) > addr)
			return pm;
	}
	return NULL;
}

void *mmap_get_block(struct process *proc, void *addr)
{
	void *ret;

	if ((ret = mmap_get_block_l(&proc->mm_stack, addr)))
		return ret;
	if ((ret = mmap_get_block_l(&proc->mm_heap, addr)))
		return ret;
	if (proc->mm_code.v_addr <= addr && (void *)proc->mm_code.v_addr + (proc->mm_code.size << 12) > addr)
		return &proc->mm_code;
	return NULL;
}

int mmap_is_free(struct process *proc, void *addr)
{
	if (mmap_get_block(proc, addr) == NULL)
		return 1;
	return 0;
}

void *mmap(struct process *proc, void *addr, size_t size, int prot, int flags, int fd, size_t offset)
{
	int err = 0;

	if (size == 0 || (size_t)addr & PAGE_FLAG)
	{
		err = -EINVAL;
		goto err;
	}
	size = mmap_get_number_of_page(size);
	unsigned pflags = PROC_MEM_ADD_HEAP;
	if (proc == current)
		pflags |= PROC_MEM_ADD_IMEDIATE;

	unsigned mflags = PAGE_PRESENT;
	if (prot & PROT_READ)
		mflags |= PAGE_USER_SUPERVISOR;
	if (prot & PROT_WRITE)
		mflags |= PAGE_USER_SUPERVISOR | PAGE_WRITE;
	if (prot & PROT_EXEC)
		mflags |= PAGE_USER_SUPERVISOR;

	if (!(flags & MAP_ANON) || fd != -1 || offset != 0)
	{
		err = -ENOSYS; // TODO for file descriptor
		goto err;
	}

	void *max_addr = (void*)(((size_t)~0) << 12);
	if (!(flags & MAP_KERNEL_SPACE))
		max_addr = (void*)0xC0000000 - (1 << 12) - (2 << 12);// TODO stack place

	if (flags & MAP_FIXED)
	{
		if (addr == NULL || addr >= max_addr || addr + (size << 12) >= max_addr || addr >= addr + (size << 12))
		{
			err = -EINVAL;
			goto err;
		}
		size_t find = 0;
		void *addr2 = addr;
		while ((err = mmap_is_free(proc, addr2)) == 1)
		{
			find++;
			if (find == size)
				break;
			addr2 += 1 << 12;
			if (addr2 > max_addr)
			{
				err = -EINVAL;
				break;
			}
		}
		if (err == 1)
			err = 0;
		else
		{
			err = -EINVAL;
			goto err;
		}
	}
	else
	{
		if (addr == NULL || addr >= max_addr)
			addr = max_addr;
		void *already = addr;
		size_t find = 0;
		while ((err = mmap_is_free(proc, addr)) >= 0)
		{
			if (err == 1)
			{
				find++;
				if (find == size)
					break;
			}
			else
				find = 0;
			addr -= 1 << 12;
			if (addr == NULL)
			{
				addr = max_addr;
				find = 0;
			}
			if (already == addr)
			{
				err = -ENOMEM;
				goto err;
			}
		}
		if (err == 1)
			err = 0;
		else
			goto err;
	}

	if ((err = process_memory_add(proc, size, addr, mflags, pflags)))
		goto err;
	return addr;
err:
	return (void*)err;
}

int munmap(struct process *proc, void *addr, size_t size, int flags)
{
	int ret = 0;
	size_t size_before;
	size_t size_after;
	struct map_memory *new_map = NULL;

	if (size == 0 || (size_t)addr & PAGE_FLAG || addr + size <= addr)
	{
		ret = -EINVAL;
		goto end;
	}

	void *max_addr = (void*)(((size_t)~0) << 12);
	if (!(flags & MAP_KERNEL_SPACE))
		max_addr = (void*)0xC0000000 - (1 << 12);
	if (addr + size > max_addr)
	{
		ret = -EINVAL;
		goto end;
	}

	size = mmap_get_number_of_page(size);
	struct map_memory *block = mmap_get_block(proc, addr);
	if (block == NULL)
	{
		ret = -EINVAL;
		goto end;
	}

	size_before = (addr - block->v_addr) >> 12;
	if (block->size - size_before <= size)
	{
		//if ((ret = munmap(proc, addr + (block->size - size_before) << 12, (size - block->size - size_before) << 12, flags)))
		//	goto end;
		size = block->size - size_before; // TODO not recursive
	}
	size_after = block->size - size - size_before;

	if (size_after)
	{
		if ((new_map = kmalloc(sizeof(*new_map))) == NULL)
		{
			ret = -ENOMEM;
			goto end;
		}
		new_map->v_addr = block->v_addr + ((size + size_before) << 12);
		new_map->p_addr = block->p_addr + ((size + size_before) << 12);
		new_map->size = size_after;
		new_map->flags = block->flags;
	}

	if (proc == current)
	{
		if ((ret = page_map_range(NULL, block->v_addr + (size_after << 12), PAGE_NOTHING, size)))
			goto end;
	}

	free_phys_block(block->p_addr + (size_after << 12), size); // TODO MULTI THREAD

	if (new_map)
		list_add(&new_map->plist, &block->plist); // TODO MULTI THREAD
	if (size_before == 0)
	{
		list_del(&block->plist);
		kfree(block);
	}
	else
		block->size = size_before;
end:
	return ret;
}

struct process *process_get_with_pid(pid_t pid)
{
	struct list_head *l;
	struct process *p;

	list_for_each(l, &process_list)
	{
		p = list_entry(l, struct process, plist);
		if (p->pid == pid)
			return p;
	}
	return NULL;
}

struct process	*process_ini_kern(u32 *v_addr, void* function, size_t size)
{
	int err;
	struct process *proc = NULL;

	if ((proc = process_new()) == NULL)
		goto err;
	if ((err = process_alloc_pid(&proc->pid)))
		goto err;

	if (process_memory_add(proc, size, v_addr, PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, PROC_MEM_ADD_IMEDIATE | PROC_MEM_ADD_CODE))
		goto err;
	memcpy((void*)((size_t)v_addr & PAGE_ADDR), (void*)((size_t)function & PAGE_ADDR), size + ((size_t)v_addr & PAGE_FLAG));
	if (process_memory_add(proc, 2 << 12, (void *)0xC0000000 - (2 << 12), PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR, PROC_MEM_ADD_IMEDIATE | PROC_MEM_ADD_STACK))
		goto err;

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
err:
	if (proc != NULL)
		free_process(proc);
	return NULL;
}
