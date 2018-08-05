#ifndef VIRT_MEM_MANAGEMENT
# define VIRT_MEM_MANAGEMENT

# include "printk.h"
# include <stdint.h>
# include <stddef.h>
# include <stdbool.h>
# include "typedef.h"
# include "position.h"
# include "page.h"
# include "list.h"

# define MALLOC_ALIGN(P) (void*)(((size_t)(P) % 16) ? (size_t)(P) - ((size_t)(P) % 16) + 16: (size_t)(P))

struct heap_list
{
	size_t			page_size : 31;
	unsigned		virtual : 1;
	struct list_head	list;
}	__attribute__((aligned(32)));

struct	alloc_header
{
	size_t		size : 31;
	unsigned	free : 1;
}	__attribute__((aligned(16)));

struct heap_list	heap_start;

unsigned char	mm_virt_heap[HEAP_SIZE / 8];

void *vmalloc(size_t size);
void *kmalloc(size_t size);
void heap_setup(void);

#endif
