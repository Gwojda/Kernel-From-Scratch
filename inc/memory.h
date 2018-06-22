#ifndef MEMORY_H
# define MEMORY_H

# include "multiboot2.h"
# include "typedef.h"
# include "printk.h"
# include "list.h"

void memory_init(unsigned long magic, unsigned long addr);

#endif
