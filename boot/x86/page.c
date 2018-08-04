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
	if ((size_t)ptr & PAGE_FLAG)
		kern_panic("FATAL ERROR: page addr %p", ptr);
	if (flag & PAGE_ADDR) 
		kern_panic("FATAL ERROR: flag");
	if (index >= 1024)
		kern_panic("FATAL ERROR: index too large %d", index);
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
	{
		page_entry_set(table, i, i << 12, flag);
		mm_bitmap[ACCESS_BITMAP_BY_ADDR((i << 12))] |= 1 << (KERNEL_GET_VIRTUAL(i << 12) % 8);
	}
}

void page_setup_kernel_section(uint32_t *table)
{
	page_entry_clear(table);

	/*
	 * Some page use in kernel code
	 */
	page_entry_set(table, TABLE_ENTRY(0x00000000), 0x00000000, PAGE_WRITE | PAGE_PRESENT); // gdt
	mm_bitmap[ACCESS_BITMAP_BY_ADDR(0x00000000)] |= 1 << (KERNEL_GET_VIRTUAL(0x00000000) % 8);
	page_entry_set(table, TABLE_ENTRY(0x000B8000), 0x000B8000, PAGE_WRITE | PAGE_PRESENT); // vga
	mm_bitmap[ACCESS_BITMAP_BY_ADDR(0x000B8000)] |= 1 << (KERNEL_GET_VIRTUAL(0x000B8000) % 8);
	// we'll need to map here idt
	/*
	 * here we go, let's map the kernel !
	*/
	page_entry_set_range(table, TABLE_ENTRY(KERNEL_REAL_START), PAGE_ALIGN(KERNEL_REAL_END) >> 12, PAGE_WRITE | PAGE_PRESENT);
	
	page_entry_set_range(table, TABLE_ENTRY(KERNEL_REAL_TEXT_START), PAGE_ALIGN(KERNEL_REAL_TEXT_END) >> 12, PAGE_PRESENT);
	page_entry_set_range(table, TABLE_ENTRY(KERNEL_REAL_RODATA_START), PAGE_ALIGN(KERNEL_REAL_RODATA_END) >> 12, PAGE_PRESENT);
	page_entry_set_range(table, TABLE_ENTRY(KERNEL_REAL_DATA_START), PAGE_ALIGN(KERNEL_REAL_DATA_END) >> 12, PAGE_WRITE | PAGE_PRESENT);
	page_entry_set_range(table, TABLE_ENTRY(KERNEL_REAL_BSS_START), PAGE_ALIGN(KERNEL_REAL_BSS_END) >> 12, PAGE_WRITE | PAGE_PRESENT);
//	physical address from swap page is now free
	mm_bitmap[ACCESS_BITMAP_BY_ADDR(KERNEL_GET_REAL(page_swap))] |= 1 << (KERNEL_GET_REAL(page_swap) % 8);
}

uint32_t *access_table_with_physical(void *empty_static_page, uint32_t *physical)
{
	unsigned int i = KERNEL_GET_REAL(empty_static_page);
	page_entry_set(kernel_page, i >> 12, physical, PAGE_WRITE | PAGE_PRESENT);
	return (empty_static_page);
}

void	init_free_vm(void)
{
	free_vm.vm_start = 0x00000000;
	free_vm.vm_end = 0xFFFFFFFF;
	INIT_LIST_HEAD(&free_vm.list);
}

void	*get_phys_block(size_t nb_pages)
{
	size_t	i = LOW_MEMORY_SIZE >> 12;
	size_t	following_bits = 0;
	size_t	j;

	while (i < MAX_RAM_PAGE / 8)
	{
		j = 0;
		while (j < 8)
		{
			if (following_bits == nb_pages)
				return ((((i - (following_bits / 8)) << 3) + (following_bits % 8)) << 12);
			if (mm_bitmap[i] & (1 << j))
				++following_bits;
			else
				following_bits = 0;
			++j;
		}
		++i;
	}
	return (NULL);
}

int page_map(void *phy_addr, void *virt_addr, unsigned int flag)
{
	if ((uint32_t)virt_addr & PAGE_FLAG)
		return 0;
	if ((uint32_t)phy_addr & PAGE_FLAG)
		return 0;
	if (flag & PAGE_ADDR)
		return 0;
	if (!(flag & PAGE_PRESENT))
		return 0;

	unsigned int page_directory_index = (size_t)virt_addr >> 22;
	unsigned int page_table_index = ((size_t)virt_addr >> 12) & 0x03FF;

	uint32_t *page_directory = KERNEL_GET_VIRTUAL(page_directory_get());//
	uint32_t *directory_entry = &(page_directory[page_directory_index]);

	if (*directory_entry & PAGE_SIZE)
		return 0;
	if (!(*directory_entry & PAGE_PRESENT))
	{
		if (!(*directory_entry = get_phys_block(1)))
			return 0;
		*directory_entry |= PAGE_PRESENT | PAGE_WRITE;
		page_directory_reset(); // page update
	}

	uint32_t *page_table = access_table_with_physical(page_swap, *directory_entry & PAGE_ADDR);
	page_directory_reset(); // page update
	page_entry_set(page_table, page_table_index, phy_addr, flag);
	page_directory_reset(); // page update

	return 1;
}

int page_unmap(void *virt_addr)
{
	if ((uint32_t)virt_addr & PAGE_FLAG)
		return 0;

	unsigned int page_directory_index = (size_t)virt_addr >> 22;
	unsigned int page_table_index = ((size_t)virt_addr >> 12) & 0x03FF;

	uint32_t *page_directory = KERNEL_GET_VIRTUAL(page_directory_get());//
	uint32_t *directory_entry = &(page_directory[page_directory_index]);

	if (*directory_entry & PAGE_SIZE)
		return 0;
	if (!(*directory_entry & PAGE_PRESENT))
		return 0;

	uint32_t *page_table = access_table_with_physical(page_swap, *directory_entry & PAGE_ADDR);
	page_directory_reset(); // page update
	page_entry_set(page_table, page_table_index, NULL, PAGE_NOTHING);

	unsigned i = 0;
	while (i < 1024)
	{
		if (page_table[i] & PAGE_PRESENT)
		{
			page_directory_reset(); // page update
			return 1;
		}
		i++;
	}

	//*directory_entry &= ~PAGE_PRESENT;
	//remove_phys_block(*directory_entry & PAGE_ADDR);
	//*directory_entry = PAGE_NOTHING;

	page_directory_reset(); // page update

	return 1;
}

int page_info(void *virt_addr)
{
	printk("page_info %p\n", virt_addr);

	if ((uint32_t)virt_addr & PAGE_FLAG)
	{
		printk("  page not allign\n");
		return 0;
	}

	unsigned int page_directory_index = (size_t)virt_addr >> 22;
	unsigned int page_table_index = ((size_t)virt_addr >> 12) & 0x03FF;

	uint32_t *page_directory = KERNEL_GET_VIRTUAL(page_directory_get());//
	uint32_t *directory_entry = &(page_directory[page_directory_index]);

	printk("  ---GSDACWRP\n");
	printk("  page directory %p\n", page_directory);
	printk("  D %b\n", *directory_entry & PAGE_FLAG);
	printk("  - %p\n", *directory_entry & PAGE_ADDR);
	if (!(*directory_entry & PAGE_PRESENT) || *directory_entry & PAGE_SIZE)
		return 1;

	uint32_t *page_table = access_table_with_physical(page_swap, *directory_entry & PAGE_ADDR);
	page_directory_reset(); // page update
	printk("  page table %p\n", page_table);
	printk("  E %b\n", page_table[page_table_index] & PAGE_FLAG);
	printk("  - %p\n", page_table[page_table_index] & PAGE_ADDR);

	return 1;
}


/*void	heap_setup(void)
{
	void	*table_phys_addr = get_phys_block(4096);
	void	*heap_phys_addr = get_phys_block(HEAP_SIZE);
	void	*table_virt_addr = PAGE_ALIGN(KERNEL_END);	// the new table entry for mapping the heap is declared after the kernel
	//pager virtuellement les addresses de HEAP_START a HEAP_END sur ses addresses physiques puis les linker au page directory. notre heap est ok, maintenant faire un kmalloc pour ensuite allouer un nouveau block de vm_area et maj les plages disponibles. faire un brk ?
//	printk(" TABLE_ENTRY((size_t)table_virt_addr) = %x\n", TABLE_ENTRY((size_t)table_virt_addr));
	printk("table_phys_addr = %x\n", table_phys_addr);
	printk("itable_virt_addr = %x\n", table_virt_addr);

	page_entry_set(kernel_page, TABLE_ENTRY((size_t)table_virt_addr), table_phys_addr, PAGE_WRITE | PAGE_PRESENT);
	for (unsigned int i = 0; i < 4096; ++i)
	{
		((char *)table_virt_addr)[i] = 0;
//		printk("((size_t)HEAP_START + i) >> 12= %x i = %d\n", TABLE_ENTRY((size_t)HEAP_START + (i << 12)), i);
//		page_entry_set(table_virt_addr, TABLE_ENTRY((size_t)HEAP_START + (i << 12)), (size_t)heap_phys_addr + (i << 12), PAGE_WRITE | PAGE_PRESENT);
	}
//	page_entry_set(page_directory, (size_t)HEAP_START >> 22, KERNEL_GET_REAL(kernel_page),PAGE_PRESENT);
}*/

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

	init_free_vm();

//	heap_setup();
//	to test page_get_entry_with_virtual
	//page_get_entry_with_virtual(page_directory);

	uint32_t *tt = get_phys_block(1);
	int *ptr = 0xF0000000;
	printk("tt %p\n", tt);
	printk("ptr %p\n", tt);
	printk("res %d\n", page_map(tt, ptr, PAGE_WRITE | PAGE_PRESENT));
	printk("res %d\n", ptr[0]);
	//ptr[0] = 42;
	printk("res %d\n", ptr[0]);
	page_info(ptr);
	printk("res %d\n", page_unmap(ptr));

	//access_table_with_physical(page_swap, kernel_page);
	//printk("page %p\n", page_get_entry_with_virtual(kernel_page));
}
