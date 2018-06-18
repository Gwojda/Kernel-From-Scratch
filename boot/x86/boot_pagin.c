#include <stdint.h>
#include <stddef.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t kernel_page[1024] __attribute__((aligned(4096)));

void setup_directory(uint32_t *directory)
{
	unsigned i;
	for (i = 0; i < 1024; i++)
	{
		// R/W only for supervisor
		// page table not present
		directory[i] = 2;
	}
}

void setup_page_table(uint32_t *directory, unsigned int index, uint32_t *table)
{
	unsigned int i;
	for(i = 0; i < 1024; i++)
	{
		table[i] = 2;
	}
	directory[index] = ((size_t)table) | 3;
}

void setup_page_page(uint32_t *table, unsigned int index, void *ptr, unsigned int flag)
{
	table[index] = ((size_t)ptr) | flag;
}

void *setup_pagin(void)
{
	while (1)
		;
	setup_directory(page_directory);
	setup_page_table(page_directory, 0xC0000000 >> 22, kernel_page);
	setup_page_page(kernel_page, 0xC0000000 >> 12, 0x00000000, 2);
	return page_directory;
}
