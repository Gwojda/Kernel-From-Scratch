#ifndef PHYS_MEM_MANAGEMENT
# define PHYS_MEM_MANAGEMENT

# include "printk.h"
# include <stdint.h>
# include <stddef.h>
# include <stdbool.h>
# include "typedef.h"
# include "position.h"

# define MAX_RAM_PAGE 0x100000
# define HIGH_MEMORY_BEGIN 0x100000

# define LOW_MEMORY_SIZE	(0x9FC00)

unsigned char	mm_bitmap[MAX_RAM_PAGE / 8];

void free_phys_block(void *start_phys_addr, size_t pages_nb);
void *get_phys_block(size_t nb_pages);

#endif
