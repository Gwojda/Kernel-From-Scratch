#ifndef PAGE_H
# define PAGE_H

# define PAGE_FLAG		0b111111111111	// ALL FLAG
# define PAGE_ADDR		(~PAGE_FLAG)	// ADDR
# define PAGE_AVAILABLE		0b111000000000	// Available
# define PAGE_GLOBAL		0b000100000000	// A
# define PAGE_SIZE		0b000010000000	// PS
# define PAGE_DIRTY		0b000001000000	// D
# define PAGE_ACCESS		0b000000100000	// A
# define PAGE_CACHE_DISABLE	0b000000010000	// PCD
# define PAGE_WRITE_THROUGH	0b000000001000	// PWT
# define PAGE_USER_SUPERVISOR	0b000000000100	// U/S
# define PAGE_WRITE		0b000000000010	// R/W
# define PAGE_PRESENT		0b000000000001	// P
# define PAGE_NOTHING		0b000000000000

# define PAGE_GET_ADDR(P) (void*)((uint32_t)(P) & PAGE_ADDR)

# define TABLE_ENTRY(P) (((size_t)(P) & 0x003FF000) >> 12)

# include "multiboot2.h"
# include "typedef.h"
# include "printk.h"
# include "panic.h"
# include "phys_mem_management.h"
# include "virt_mem_management.h"

extern uint32_t page_directory[1024];
extern uint32_t page_swap[1024];

int page_map(void *physical_addr, void *virtual_addr, unsigned flags);
int page_unmap(void *virt_addr);

struct page_info_data
{
	void		*target;
	void		*target_page;
	int		error;

	void		*page_directory;
	uint32_t	page_directory_entry;

	int		have_page_entry;
	void		*page_table;
	uint32_t	page_table_entry;
};

int page_info(void *virt_addr, struct page_info_data *ret);
int page_info_data_display(struct page_info_data *ret);
int page_info_display(void *virt_addr);

void page_setup(void);

void memory_init(unsigned long magic, unsigned long addr);

#endif
