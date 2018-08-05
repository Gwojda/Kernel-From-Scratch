#include "virt_mem_management.h"

void heap_setup(void)
{
	INIT_LIST_HEAD(&heap_start.list);
	heap_start.page_size = 0;
	heap_start.virtual = 0;
	for (size_t i = 0 ; i < HEAP_SIZE / 8 ; ++i)
		mm_virt_heap[i] = -1;
}

static void	alloc_virt_block(void *start_virt_addr, size_t pages_nb)
{
	while (pages_nb / 8)
	{
		mm_virt_heap[(start_virt_addr - HEAP_START) >> 8] = 0;
		start_virt_addr += 8 * 4096;
		pages_nb -= 8;
	}
	while (pages_nb)
	{
		mm_virt_heap[(start_virt_addr - HEAP_START) >> 8] &= ~(1 << (pages_nb - 1));
		start_virt_addr += 4096;
		--pages_nb;
	}
}

static void	free_virt_block(void *start_virt_addr, size_t pages_nb)
{
	while (pages_nb / 8)
	{
		mm_virt_heap[(start_virt_addr - HEAP_START) >> 8] = -1;
		start_virt_addr += 8 * 4096;
		pages_nb -= 8;
	}
	while (pages_nb)
	{
		mm_virt_heap[(start_virt_addr - HEAP_START) >> 8] |= 1 << pages_nb;
		start_virt_addr += 4096;
		--pages_nb;
	}
}

static void *find_free_virt_addr(size_t size)
{
	size_t	i = 0;
	size_t	following_bits = 0;
	size_t	j;

	size = (size_t)PAGE_ALIGN(size) >> 12;
	while (i < HEAP_SIZE / 8)
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
	((struct alloc_header *)begin_alloc_header)->size = size - sizeof(struct heap_list);
	((struct alloc_header *)begin_alloc_header)->free = 1;
}

void	*try_allocate(struct heap_list *heap_entry, size_t alloc_size)
{
	struct alloc_header	*head = (void *)heap_entry + sizeof(struct heap_list);

	alloc_size = (size_t)MALLOC_ALIGN(alloc_size);
	while ((void *)head < (void *)heap_entry + heap_entry->page_size)
	{
		if (alloc_size < head->size)
		{
			printk("alloc header = %d\n",sizeof(struct alloc_header));
			if (head->size - (sizeof(struct alloc_header) + alloc_size) > sizeof(struct alloc_header) + 16)
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

	list_for_each_entry(heap_entry, &heap_start.list, list)
	{
		if (!heap_entry->virtual && heap_entry->page_size && (ret = try_allocate(heap_entry, size)))
				return ret;
	}
	
	if (!(new_virt_addr = find_free_virt_addr(size)))
		goto err;	//no more virt addr available
	if (!(new_phys_addr = get_phys_block((size_t)PAGE_ALIGN(size) >> 12)))
		goto err1;	//no more aligned phys addr
	if (!page_map(new_phys_addr, new_virt_addr, PAGE_WRITE | PAGE_PRESENT))
		goto err2;
	init_new_allocated_block(new_virt_addr, (size_t)PAGE_ALIGN(size), 0);
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
	void			*new_phys_addr;

	list_for_each_entry(heap_entry, &heap_start.list, list)
	{
		if (heap_entry->virtual && (ret = try_allocate(heap_entry, size)))
				return ret;
	}
	if (!(new_virt_addr = find_free_virt_addr(size)))
		goto err;	//no more virt addr available
	for (void *tmp_virt_addr = new_virt_addr ; tmp_virt_addr < new_virt_addr + (size_t)PAGE_ALIGN(size) ; tmp_virt_addr += 4096)
	{
		if (!(new_phys_addr = get_phys_block(1)))
			goto err1;	//no more phys addr
		if (!page_map(new_phys_addr, new_virt_addr, PAGE_WRITE | PAGE_PRESENT))
			goto err1;
	}
	init_new_allocated_block(new_virt_addr, (size_t)PAGE_ALIGN(size), 1);
	return (try_allocate(heap_entry, size));

err1:
	free_phys_block(new_phys_addr, (size_t)PAGE_ALIGN(size) >> 12);
	// Not good, i need reference on all my physical allocated memory....
	// Allocate an list with kmalloc with all ptr reference, and kfree this list after free the physical block ?

	free_virt_block(new_virt_addr, (size_t)PAGE_ALIGN(size) >> 12);

err:
	return (NULL);
}
