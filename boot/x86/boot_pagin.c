#include "kernel.h"

uint32_t *page_directory;
uint32_t kernel_page[1024] __attribute__((aligned(4096)));

void *change_root_pagin(void*);
void *get_currant_page_directory(void);

#define PAGE_ADDR		(~0xFFF)	// ADDR
#define PAGE_FLAG		0xFFF		// ADDR
#define PAGE_AVAILABLE		0b111000000000	// Available
#define PAGE_GLOBAL		0b000100000000	// A
#define PAGE_SIZE		0b000010000000	// PS
#define PAGE_DIRTY		0b000001000000	// D
#define PAGE_ACCESS		0b000000100000	// A
#define PAGE_CACHE_DISABLE	0b000000010000	// PCD
#define PAGE_WRITE_THROUGH	0b000000001000	// PWT
#define PAGE_USER_SUPERVISOR	0b000000000100	// U/S
#define PAGE_READ_WRITE		0b000000000010	// R/W
#define PAGE_PRESENT		0b000000000001	// P
#define PAGE_NOTHING		0b000000000000

void setup_page_table(uint32_t *table)
{
	unsigned int i;
	for(i = 0; i < 1024; i++)
		table[i] = 0;
}

void setup_page_page(uint32_t *table, unsigned int index, void *ptr, unsigned int flag)
{
	if ((size_t)ptr & PAGE_FLAG) {
		printk("FATAL ERROR: page addr");
		while (1)
			;
	}
	if (flag & PAGE_ADDR) {
		printk("FATAL ERROR: flag");
		while (1)
			;
	}
	if (index >= 1024) {
		printk("FATAL ERROR: index too large");
		while (1)
			;
	}
	table[index] = ((size_t)ptr) | flag;
}

void setup_page_disable(uint32_t *table, unsigned int index)
{
	setup_page_page(table, index, 0x00000000, PAGE_NOTHING);
}

void setup_pagin(void)
{
	page_directory = get_currant_page_directory();

	// We remove the old identity page mapping
	setup_page_disable(page_directory, 0x00000000 >> 22);
	setup_page_page(page_directory, (0xC0000000 >> 22), 0x00000000, PAGE_SIZE | PAGE_READ_WRITE | PAGE_PRESENT);
	//setup_page_page(page_directory, (0xC0400000 >> 22), 0x00400000, PAGE_SIZE | PAGE_READ_WRITE | PAGE_PRESENT);

	/*printk("%p\n", kernel_page);
	setup_page_table(kernel_page);
	setup_page_page(kernel_page, (0xC0000000 >> 12) & 1024, 0x00000000, PAGE_READ_WRITE | PAGE_PRESENT);*/

	change_root_pagin(page_directory);
}
