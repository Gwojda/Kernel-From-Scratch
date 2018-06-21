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
# define PAGE_READ_WRITE	0b000000000010	// R/W
# define PAGE_PRESENT		0b000000000001	// P
# define PAGE_NOTHING		0b000000000000

extern uint32_t page_directory[1024];

void page_directory_set(uint32_t *ptr);
uint32_t *page_directory_get(void);

void page_entry_clear(uint32_t *table);
void page_entry_set(uint32_t *table, unsigned int index, void *ptr, unsigned int flag);
void page_entry_remove(uint32_t *table, unsigned int index);

void page_setup(void);

#endif
