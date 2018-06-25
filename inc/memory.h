#ifndef MEMORY_H
# define MEMORY_H

# include "multiboot2.h"
# include "typedef.h"
# include "printk.h"
# include "list.h"

# define MAX_RAM_PAGE 0x100000
# define HIGH_MEMORY_BEGIN 0x100000

void memory_init(unsigned long magic, unsigned long addr);

typedef struct	s_memory
{
	unsigned long	lowMemorySize;

	void		*highMemory;
	unsigned long	highMemorySize;

}		t_memory;

extern t_memory	mm_info;

#endif
