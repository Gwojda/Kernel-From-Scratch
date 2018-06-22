#include "kernel.h"

uint32_t kernel_page[1024] __attribute__((aligned(4096)));

void page_directory_reset(void)
{
	page_directory_set(page_directory_get());
}

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

void page_entry_set_range(uint32_t *table, unsigned int from, unsigned int to, unsigned flag)
{
	unsigned int i;
	for (i = from; i < to; i++)
		page_entry_set(kernel_page, i, 4096 * i, flag);
}

void page_setup_kernel_section(uint32_t *table)
{
	page_entry_clear(table);

	/*
	 * Some page use in kernel code
	 */
	page_entry_set(table, 0x00000000 >> 12, 0x00000000, PAGE_WRITE | PAGE_PRESENT); // gdt
	page_entry_set(table, 0x000B8000 >> 12, 0x000B8000, PAGE_WRITE | PAGE_PRESENT); // vga

	/*
	 * We map the all binary, becose of strange setion in binary
	 * The end in not aligned, so
	 */
	size_t kernel_end_page = KERNEL_REAL_END;
	if ((kernel_end_page >> 12) << 12 != kernel_end_page >> 12)
		kernel_end_page = kernel_end_page + (1 << 12);
	page_entry_set_range(table, KERNEL_REAL_START >> 12, kernel_end_page >> 12, PAGE_WRITE | PAGE_PRESENT);

	page_entry_set_range(table, KERNEL_REAL_BSS_START >> 12, KERNEL_REAL_BSS_END >> 12, PAGE_WRITE | PAGE_PRESENT);
	page_entry_set_range(table, KERNEL_REAL_TEXT_START >> 12, KERNEL_REAL_TEXT_END >> 12, PAGE_PRESENT);
	page_entry_set_range(table, KERNEL_REAL_DATA_START >> 12, KERNEL_REAL_DATA_END >> 12, PAGE_WRITE | PAGE_PRESENT);
	page_entry_set_range(table, KERNEL_REAL_RODATA_START >> 12, KERNEL_REAL_RODATA_END >> 12, PAGE_PRESENT);
}

/*uint32_t page_get_entry_with_virtual(void *addr)
{
	unsigned int page_directory_index = (size_t)addr >> 22;
	unsigned int page_table_index = (size_t)addr >> 12 & 0x03FF;

	uint32_t *page_directory = KERNEL_GET_VIRTUAL(page_directory_get());
	uint32_t directory_entry = page_directory[page_directory_index];
	if (directory_entry & PAGE_SIZE)
		return (directory_entry);

	uint32_t *page_table = KERNEL_GET_VIRTUAL(directory_entry & PAGE_ADDR);
	return page_table[page_table_index];
}

int page_map(void *phy_addr, void *virt_addr, unsigned int flag)
{
	
}*/

void page_setup(void)
{
	// We remove the old identity page mapping
	page_entry_remove(page_directory, 0x00000000 >> 22);

	// We don't want a sigle 4M section for map the kernel, so we buil the page
	page_setup_kernel_section(kernel_page);
	// We inject the page
	page_entry_set(page_directory, (0xC0000000 >> 22), KERNEL_GET_REAL(kernel_page), /*PAGE_WRITE |*/ PAGE_PRESENT);
	// We have all the thing to stop use
	page_directory_reset();
	//printk("page %p\n", page_get_entry_with_virtual(kernel_page));
}
