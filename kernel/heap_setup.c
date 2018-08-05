#include "position.h"
#include "page.h"

struct heap_list
{
	struct heap_list	*next;
	size_t			page_size;
	unsigned		virtual;
	struct heap_alloc_list	*alloc;
};

struct heap_alloc_list
{
	struct heap_alloc_list	*next;
	unsigned		free;
};

struct heap_list heap_start;

void heap_setup(void)
{
	return;
}

size_t heap_alloc_start(struct heap_alloc_list *l)
{
	return l + sizeof(*l);
}
size_t heap_alloc_size(struct heap_alloc_list *l)
{
	return l->next - heap_alloc_start(l);
}

void *kmalloc(int virtual, size_t size)
{
	struct heap_list	**cur = heap_start;
	struct heap_alloc_list	**alloc;

	while (*heap_start)
	{
		alloc = &((*cur)->alloc);
		if (alloc->virtual == 0 || virtual)
			while (*alloc)
			{
				if (alloc->free && heap_alloc_size(*alloc) >= size)
				{
					(*alloc)->free = 0;
					return (heap_alloc_start(*alloc));
				}
				alloc = &((*alloc)->next);
			}
		cur = &((*cur)->next);
	}
	if (!(heap_start = get_phys_block(1)))
		return NULL;
}
