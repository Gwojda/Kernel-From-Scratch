#include "kernel.h"

//uint32_t kernel_page[1024] __attribute__((aligned(4096)));

void page_entry_clear(uint32_t *table)
{
	unsigned int i;
	for(i = 0; i < 1024; i++)
		table[i] = 0;
}

void page_entry_set(uint32_t *table, unsigned int index, void *ptr, unsigned int flag)
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

void page_entry_remove(uint32_t *table, unsigned int index)
{
	page_entry_set(table, index, 0x00000000, PAGE_NOTHING);
}

void page_setup(void)
{
	// We remove the old identity page mapping
	page_entry_remove(page_directory, 0x00000000 >> 22);
	page_entry_set(page_directory, (0xC0000000 >> 22), 0x00000000, PAGE_SIZE | PAGE_READ_WRITE | PAGE_PRESENT);
	//setup_page_page(page_directory, (0xC0400000 >> 22), 0x00400000, PAGE_SIZE | PAGE_READ_WRITE | PAGE_PRESENT);

	/*printk("%p\n", kernel_page);
	setup_page_table(kernel_page);
	setup_page_page(kernel_page, (0xC0000000 >> 12) & 1024, 0x00000000, PAGE_READ_WRITE | PAGE_PRESENT);*/
	page_directory_set((size_t)page_directory - 0xC0000000);
}
