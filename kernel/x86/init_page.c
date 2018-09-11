#include "kernel.h"

uint32_t kernel_page[1024] __attribute__((aligned(4096)));
uint32_t page_swap[1024] __attribute__((aligned(4096)));

void page_setup_kernel_section(uint32_t *table)
{
	page_entry_clear(table);

	/*
	 * Some page use in kernel code
	 */
	page_entry_set(table, TABLE_ENTRY(0x00000000), (void*)0x00000000, PAGE_WRITE | PAGE_PRESENT); // gdt
	mm_bitmap[ACCESS_BITMAP_BY_ADDR(0x00000000)] &= ~(1 << (0x00000000 % 8));
	page_entry_set(table, TABLE_ENTRY(0x000B8000), (void*)0x000B8000, PAGE_WRITE | PAGE_PRESENT); // vga
	mm_bitmap[ACCESS_BITMAP_BY_ADDR(0x000B8000)] &= ~(1 << (0x000B8000 % 8));
	page_entry_set(table, TABLE_ENTRY(0x000E0000), (void*)0x000E0000, PAGE_WRITE | PAGE_PRESENT); // vga
	mm_bitmap[ACCESS_BITMAP_BY_ADDR(0x0000E000)] &= ~(1 << (0x000E0000 % 8));
	page_entry_set_range(table, TABLE_ENTRY(0x000E0000), TABLE_ENTRY(0x000100000), PAGE_PRESENT);

	// we'll need to map here idt
	/*
	 * here we go, let's map the kernel !
	*/
	page_entry_set_range(table, TABLE_ENTRY(KERNEL_REAL_START), TABLE_ENTRY(PAGE_ALIGN(KERNEL_REAL_END)), PAGE_WRITE | PAGE_PRESENT);

	page_entry_set_range(table, TABLE_ENTRY(KERNEL_REAL_TEXT_START), TABLE_ENTRY(PAGE_ALIGN(KERNEL_REAL_TEXT_END)), PAGE_PRESENT);
	page_entry_set_range(table, TABLE_ENTRY(KERNEL_REAL_RODATA_START), TABLE_ENTRY(PAGE_ALIGN(KERNEL_REAL_RODATA_END)), PAGE_PRESENT);
	page_entry_set_range(table, TABLE_ENTRY(KERNEL_REAL_DATA_START), TABLE_ENTRY(PAGE_ALIGN(KERNEL_REAL_DATA_END)), PAGE_WRITE | PAGE_PRESENT);
	page_entry_set_range(table, TABLE_ENTRY(KERNEL_REAL_BSS_START), TABLE_ENTRY(PAGE_ALIGN(KERNEL_REAL_BSS_END)), PAGE_WRITE | PAGE_PRESENT);
}

void	page_setup(void)
{
	// We remove the old identity page mapping
	page_entry_remove(page_directory, 0x00000000 >> 22);
	// We don't want a sigle 4M section for map the kernel, so we build the page
	page_setup_kernel_section(kernel_page);
	// We inject the page
	page_entry_set(page_directory, (0xC0000000 >> 22), KERNEL_GET_REAL(kernel_page), /*PAGE_WRITE |*/ PAGE_PRESENT);
	// We have all the thing to stop use
	page_directory_reset();

	heap_setup();
}
