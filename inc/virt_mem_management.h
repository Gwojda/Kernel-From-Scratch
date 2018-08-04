#ifndef VIRT_MEM_MANAGEMENT
# define VIRT_MEM_MANAGEMENT

# include "printk.h"
# include <stdint.h>
# include <stddef.h>
# include <stdbool.h>
# include "typedef.h"
# include "list.h"

struct vm_area
{
	uint32_t		vm_start;
	uint32_t		vm_end;
	struct list_head	list;
};

struct vm_area	free_vm;

#endif
