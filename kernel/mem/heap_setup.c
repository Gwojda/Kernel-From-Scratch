#include "virt_mem_management.h"

void heap_setup(void)
{
	INIT_LIST_HEAD(&heap_start.list);
	heap_start.page_size = 0;
	heap_start.virtual = 0;
	for (size_t i = 0 ; i < HEAP_SIZE / 8 ; ++i)
		mm_virt_heap[i] = -1;
}

static void	alloc_virt_block(void *start_virt_addr, size_t size)
{
	while (size)
	{
		mm_virt_heap[(start_virt_addr - HEAP_START) >> 15] &= ~(1 << (((start_virt_addr - HEAP_START) >> 12) % 8));
		start_virt_addr += 4096;
		--size;
	}
}

static void	free_virt_block(void *start_virt_addr, size_t size)
{
	while (size)
	{
		if (mm_virt_heap[(start_virt_addr - HEAP_START) >> 15] & 1 << (((start_virt_addr - HEAP_START) >> 12) % 8))
			printk("try to free already unallocated block\n");
		mm_virt_heap[(start_virt_addr - HEAP_START) >> 15] |= 1 << (((start_virt_addr - HEAP_START) >> 12) % 8);
		start_virt_addr += 4096;
		--size;
	}
}

static void *find_free_virt_addr(size_t size)
{
	size_t	i = 0;
	size_t	following_bits = 0;
	size_t	j;

	size = (size_t)PAGE_ALIGN(size) >> 12;
	while (i < HEAP_SIZE / (8 * 4096))
	{
		j = 0;
		while (j < 8)
		{
			if (following_bits == size)
			{
				alloc_virt_block(((i * 8 + j - following_bits) << 12) + HEAP_START, following_bits);
				return ((void *)(((i * 8 + j - following_bits) << 12) + (size_t)HEAP_START));
			}
			if (mm_virt_heap[i] & (1 << j))
				++following_bits;
			else
				following_bits = 0;
			++j;
		}
		++i;
	}
	return (NULL);
}

static void	init_new_allocated_block(void *virt_addr, size_t size, unsigned virtual)
{
	void	*begin_alloc_header = virt_addr + sizeof(struct heap_list);

	((struct heap_list *)virt_addr)->page_size = size - sizeof(struct heap_list);
	((struct heap_list *)virt_addr)->virtual = virtual;
	list_add(&((struct heap_list *)virt_addr)->list, &heap_start.list);
	((struct alloc_header *)begin_alloc_header)->size = size - sizeof(struct heap_list) - sizeof(struct alloc_header);
	((struct alloc_header *)begin_alloc_header)->free = 1;
}

void	*try_allocate(struct heap_list *heap_entry, size_t alloc_size)
{
	struct alloc_header	*head = (void *)heap_entry + sizeof(struct heap_list);

	alloc_size = (size_t)MALLOC_ALIGN(alloc_size);
	while ((void *)head < (void *)heap_entry + heap_entry->page_size)
	{
		if (alloc_size < head->size && head->free)
		{
			if (head->size > sizeof(struct alloc_header) + 16)
			{
				struct alloc_header	*new_head = (void *)head + sizeof(struct alloc_header) + alloc_size;
				new_head->free = 1;
				new_head->size = head->size - (sizeof(struct alloc_header) + alloc_size);
				head->size = alloc_size;
			}
			head->free = 0;
			return ((void *)head + sizeof(struct alloc_header));
		}
		head = (void *)head + sizeof(struct alloc_header) + head->size;
	}
	return (NULL);
}

void *kmalloc(size_t size)
{
	struct heap_list	*heap_entry;
	void			*ret;
	void			*new_virt_addr;
	void			*new_phys_addr;

	if (!size)
		return (NULL);
	list_for_each_entry(heap_entry, &heap_start.list, list)
	{
		if (!heap_entry->virtual && heap_entry->page_size && (ret = try_allocate(heap_entry, size)))
			return ret;
	}
	if (!(new_virt_addr = find_free_virt_addr((size_t)PAGE_ALIGN(size + sizeof(struct heap_list) + sizeof(struct alloc_header)))))
		goto err;	//no more virt addr available
	if (!(new_phys_addr = get_phys_block((size_t)PAGE_ALIGN(size + sizeof(struct heap_list) + sizeof(struct alloc_header)) >> 12)))
		goto err1;	//no more aligned phys addr
	if (page_map_range(new_phys_addr, new_virt_addr, PAGE_WRITE | PAGE_PRESENT, (size_t)PAGE_ALIGN(size + sizeof(struct heap_list) + sizeof(struct alloc_header)) >> 12))
		goto err2;
	init_new_allocated_block(new_virt_addr, (size_t)PAGE_ALIGN(size + sizeof(struct heap_list) + sizeof(struct alloc_header)), 0);
	heap_entry = list_entry(heap_entry->list.next, typeof(*heap_entry), list);
	return (try_allocate(heap_entry, size));

err2:
	free_phys_block(new_phys_addr, (size_t)PAGE_ALIGN(size) >> 12);
err1:
	free_virt_block(new_virt_addr, (size_t)PAGE_ALIGN(size) >> 12);
err:
	return (NULL);
}

void *vmalloc(size_t size)
{
	struct heap_list	*heap_entry;
	void			*ret;
	void			*new_virt_addr;
	void			*last_virt_addr_mapped;
	void			*new_phys_addr;

	if (!size)
		return (NULL);
	list_for_each_entry(heap_entry, &heap_start.list, list)
	{
		if (heap_entry->virtual && heap_entry->page_size && (ret = try_allocate(heap_entry, size)))
			return ret;
	}
	if (!(new_virt_addr = find_free_virt_addr((size_t)PAGE_ALIGN(size + sizeof(struct heap_list) + sizeof(struct alloc_header)))))
		goto err;	//no more virt addr available
	last_virt_addr_mapped = new_virt_addr;
	for (void *tmp_virt_addr = new_virt_addr ; tmp_virt_addr < new_virt_addr + (size_t)PAGE_ALIGN(size + sizeof(struct heap_list) + sizeof(struct alloc_header)) ; tmp_virt_addr += 4096)
	{
		if (!(new_phys_addr = get_phys_block(1)))
			goto err1;	//no more phys addr
		if (page_map(new_phys_addr, tmp_virt_addr, PAGE_WRITE | PAGE_PRESENT))
			goto err2;
		last_virt_addr_mapped = tmp_virt_addr;
	}
	init_new_allocated_block(new_virt_addr, (size_t)PAGE_ALIGN(size + sizeof(struct heap_list) + sizeof(struct alloc_header)), 1);
	heap_entry = list_entry(heap_entry->list.next, typeof(*heap_entry), list);
	return (try_allocate(heap_entry, size));

err2:
	free_phys_block(new_phys_addr, 1);

err1:
	for (void *tmp_virt_addr = new_virt_addr ; tmp_virt_addr < last_virt_addr_mapped ; tmp_virt_addr += 4096)
		free_phys_block(page_get_phys(tmp_virt_addr), 1);
	free_virt_block(new_virt_addr, (size_t)PAGE_ALIGN(size) >> 12);
err:
	return (NULL);
}

static void vdefrag(struct heap_list *heap_entry)
{
	struct alloc_header	*head = (void *)heap_entry + sizeof(struct heap_list);
	struct alloc_header	*tmp;

	while ((void *)head < (void *)heap_entry + heap_entry->page_size)
	{
		if (head->free == 0)
		{
			head = (void *)head + sizeof(struct alloc_header) + head->size;
			continue ;
		}
		tmp = (void *)head + sizeof(struct alloc_header) + head->size;
		while ((void *)tmp < (void *)heap_entry + heap_entry->page_size && tmp->free)
		{
			head->size += tmp->size + sizeof(struct alloc_header);
			tmp = (void *)tmp + sizeof(struct alloc_header) + tmp->size;
		}
		head = (void *)head + sizeof(struct alloc_header) + head->size;
	}
	struct alloc_header	*start = (void *)heap_entry + sizeof(struct heap_list);
	if (heap_entry->page_size == start->size + sizeof(struct alloc_header) && start->free)
	{
		list_del(&heap_entry->list);
		for (void *tmp_virt_addr = heap_entry ; (size_t)tmp_virt_addr < (size_t)heap_entry + (size_t)PAGE_ALIGN(heap_entry->page_size) ; tmp_virt_addr += 4096)
			free_phys_block(page_get_phys(tmp_virt_addr), 1);
		free_virt_block(heap_entry, (size_t)PAGE_ALIGN(heap_entry->page_size) >> 12);
	}
}

static void kdefrag(struct heap_list *heap_entry)
{
	struct alloc_header	*head = (void *)heap_entry + sizeof(struct heap_list);
	struct alloc_header	*tmp;

	while ((void *)head < (void *)heap_entry + heap_entry->page_size)
	{
		if (head->free == 0)
		{
			head = (void *)head + sizeof(struct alloc_header) + head->size;
			continue ;
		}
		tmp = (void *)head + sizeof(struct alloc_header) + head->size;
		while ((void *)tmp < (void *)heap_entry + heap_entry->page_size && tmp->free)
		{
			head->size += tmp->size + sizeof(struct alloc_header);
			tmp = (void *)tmp + sizeof(struct alloc_header) + tmp->size;
		}
		head = (void *)head + sizeof(struct alloc_header) + head->size;
	}
	struct alloc_header	*start = (void *)heap_entry + sizeof(struct heap_list);
	if (heap_entry->page_size == start->size + sizeof(struct alloc_header) && start->free)
	{
		list_del(&heap_entry->list);
		free_virt_block(heap_entry, (size_t)PAGE_ALIGN(heap_entry->page_size) >> 12);
		free_phys_block(page_get_phys(heap_entry), (size_t)PAGE_ALIGN(heap_entry->page_size) >> 12);
	}
}

static void _free(const void *ptr, void (*defrag)(struct heap_list *))
{
	struct heap_list	*heap_entry;

	if (!ptr)
		return ;
	list_for_each_entry(heap_entry, &heap_start.list, list)
	{
		if ((size_t)ptr > (size_t)heap_entry)
		{
			struct alloc_header	*head = (void *)heap_entry + sizeof(struct heap_list);

			while ((void *)head < (void *)heap_entry + heap_entry->page_size)
			{
				if ((size_t)ptr == (size_t)head + sizeof(struct alloc_header))
				{
					head->free = 1;
					defrag(heap_entry);
					return ;
				}
				head = (void *)head + sizeof(struct alloc_header) + head->size;
			}
		}
	}
}

void kfree(const void *ptr)
{
	_free(ptr, &kdefrag);
}

void vfree(const void *ptr)
{
	_free(ptr, &vdefrag);
}

size_t ksize(const void *ptr)
{
	struct heap_list	*heap_entry;

	list_for_each_entry(heap_entry, &heap_start.list, list)
	{
		if ((size_t)ptr > (size_t)heap_entry)
		{
			struct alloc_header	*head = (void *)heap_entry + sizeof(struct heap_list);

			while ((void *)head < (void *)heap_entry + heap_entry->page_size)
			{
				if ((size_t)ptr == (size_t)head + sizeof(struct alloc_header))
					return (head->size);
				head = (void *)head + sizeof(struct alloc_header) + head->size;
			}

		}
	}
	return (0);
}

size_t vsize(const void *ptr)
{
	return (ksize(ptr));
}

