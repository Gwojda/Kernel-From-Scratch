#include "kernel.h"

uint32_t kernel_page[1024] __attribute__((aligned(4096)));
uint32_t page_swap[1024] __attribute__((aligned(4096)));

void page_directory_reset(void)
{
	page_directory_set(page_directory_get());
}

static inline void invlpg(void* m)
{
	asm volatile("invlpg (%0)" : : "b"(m) : "memory");
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
		page_entry_set(table, i, (void*)(i << 12), flag);
}

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

uint32_t *access_table_with_physical(void *empty_static_page, void *physical)
{
	void *i = KERNEL_GET_REAL(empty_static_page);
	page_entry_set(kernel_page, (size_t)i >> 12, physical, PAGE_WRITE | PAGE_PRESENT);
	invlpg(empty_static_page);
	return (empty_static_page);
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

	uint32_t *page_table = access_table_with_physical(page_swap, PAGE_GET_ADDR(*directory_entry));
	page_entry_set(page_table, page_table_index, NULL, PAGE_NOTHING);
	invlpg(virt_addr);

	unsigned i = 0;
	while (i < 1024)
	{
		if (page_table[i] & PAGE_PRESENT)
			return 1;
		i++;
	}

	*directory_entry &= ~PAGE_PRESENT;
	free_phys_block(PAGE_GET_ADDR(*directory_entry), 1);
	*directory_entry = PAGE_NOTHING;

	page_directory_reset(); // page update

	return 1;
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
		return page_unmap(virt_addr);

	unsigned int page_directory_index = (size_t)virt_addr >> 22;
	unsigned int page_table_index = ((size_t)virt_addr >> 12) & 0x03FF;

	uint32_t *page_directory = KERNEL_GET_VIRTUAL(page_directory_get());//
	uint32_t *directory_entry = &(page_directory[page_directory_index]);

	if (*directory_entry & PAGE_SIZE)
		return 0;
	if (!(*directory_entry & PAGE_PRESENT))
	{
		if (!(*directory_entry = (uint32_t)get_phys_block(1)))
			return 0;
		*directory_entry |= PAGE_PRESENT | PAGE_WRITE;
		page_directory_reset(); // page update
	}

	uint32_t *page_table = access_table_with_physical(page_swap, PAGE_GET_ADDR(*directory_entry));
	page_entry_set(page_table, page_table_index, phy_addr, flag);
	invlpg(virt_addr);

	return 1;
}

int page_map_range(void *phy_addr, void *virt_addr, unsigned flags, size_t size)
{
	size_t i;

	i = 0;
	while (i < size)
		if (page_map(phy_addr + (i << 12), virt_addr + (i << 12), flags) == 0)
			goto error;
	return 1;
error:
	i = 0;
	while (i < size)
		if (page_unmap(virt_addr + (i << 12)) == 0)
			return 0;
}

int page_info(void *virt_addr, struct page_info_data *ret)
{
	ret->target = virt_addr;
	virt_addr = ((size_t)virt_addr >> 12) << 12;
	ret->target_page = virt_addr;
	ret->error = 1;
	ret->page_directory = 0;
	ret->page_directory_entry = 0;
	ret->have_page_entry = 0;
	ret->page_table = 0;
	ret->page_table_entry = 0;

	unsigned int page_directory_index = (size_t)virt_addr >> 22;
	unsigned int page_table_index = ((size_t)virt_addr >> 12) & 0x03FF;

	uint32_t *page_directory = KERNEL_GET_VIRTUAL(page_directory_get());//
	uint32_t *directory_entry = &(page_directory[page_directory_index]);

	ret->error = 0;
	ret->page_directory = page_directory;
	ret->page_directory_entry = *directory_entry;

	if (!(*directory_entry & PAGE_PRESENT) || *directory_entry & PAGE_SIZE)
		return 1;

	uint32_t *page_table = access_table_with_physical(page_swap, PAGE_GET_ADDR(*directory_entry));

	ret->have_page_entry = 1;
	ret->page_table = page_table;
	ret->page_table_entry = page_table[page_table_index];
	return 1;
}

int page_info_data_display(struct page_info_data *ret)
{
	struct
	{
		char *mini;
		char *desc;
	} flags_info [] = {
		{"P", "Page present"},
		{"R/W", "Write permition"},
		{"U/S", "user can accesse to the page"},
		{"PWT", "Page write through"},
		{"PCD", "Page cache disable"},
		{"A", "Page have been read"},
		{"D", "Page have been writed"},
		{"PS", "Page size (4MiB)"},
		{"G", "Page global"},
		{"AV1", "AVAILABLE"},
		{"AV2", "AVAILABLE"},
		{"AV3", "AVAILABLE"}
	};
	int i;

	printk("Memory information: %p\n", ret->target);
	printk("  real page %p\n", ret->target_page);
	printk("  page_directory %p\n", ret->page_directory);
	printk("  page_directory_entry %p\n", ret->page_directory_entry);
	printk("   addr %p\n", PAGE_GET_ADDR(ret->page_directory_entry));
	printk("   flag %b\n", ret->page_directory_entry & PAGE_FLAG);
	for (i = 0; (1 << i) & PAGE_FLAG; i++)
		if ((1 << i) & (size_t)ret->page_directory_entry)
			printk("    %s(%d) -> %s\n", flags_info[i].mini, i, flags_info[i].desc);
	if (!ret->have_page_entry)
	{
		printk("  no page entry\n");
		return 1;
	}
	printk("  page_entry %p\n", ret->page_table);
	printk("  page_table_entry %p\n", ret->page_table_entry);
	printk("   addr %p\n", PAGE_GET_ADDR(ret->page_table_entry));
	printk("   flag %b\n", ret->page_table_entry & PAGE_FLAG);
	for (i = 0; (1 << i) & PAGE_FLAG; i++)
		if ((1 << i) & (size_t)ret->page_table_entry)
			printk("    %s(%d) -> %s\n", flags_info[i].mini, i, flags_info[i].desc);
	return 1;
}

int page_info_display(void *virt_addr)
{
	struct page_info_data info_data;

	page_info(virt_addr, &info_data);
	return page_info_data_display(&info_data);
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
}
