#include "kernel.h"

uint32_t kernel_page[1024] __attribute__((aligned(4096)));
uint32_t page_swap[1024] __attribute__((aligned(4096)));

static inline void invlpg(void* m)
{
	asm volatile("invlpg (%0)" : : "b"(m) : "memory");
}

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
		page_entry_set(table, i, (void*)(i << 12), flag);
}

uint32_t *access_table_with_physical(void *empty_static_page, void *physical)
{
	void *i = KERNEL_GET_REAL(empty_static_page);
	page_entry_set(kernel_page, (size_t)i >> 12, physical, PAGE_WRITE | PAGE_PRESENT);
	invlpg(empty_static_page);
	return (empty_static_page);
}

int page_unmap(void *virt_addr, unsigned flag)
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
	*directory_entry = flag;

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
		return page_unmap(virt_addr, flag);

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
		*directory_entry |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR;
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

	for (i = 0; i < size; i++)
		if (page_map(phy_addr + (i << 12), virt_addr + (i << 12), flags) == 0)
			goto error;
	return 1;
error:
	for (; i > 0; i--)
		if (page_unmap(virt_addr + (i << 12), PAGE_NOTHING) == 0)
			continue;
	return 0;
}

int page_map_at(void *virt_addr, unsigned flags, size_t nb_page)
{
	void *new_page;

	if (!(flags & PAGE_PRESENT))
		return 0;
	if (flags & PAGE_ADDR)
		return 0;
	if ((size_t)virt_addr & PAGE_FLAG)
		return 0;

	if (!(new_page = get_phys_block(nb_page)))
		return 0;
	if (page_map_range(new_page, virt_addr, flags, nb_page) == 0)
	{
		free_phys_block(new_page, nb_page);
		return 0;
	}
	return 1;
}

int page_unmap_at(void *virt_addr, unsigned flags, size_t nb_page)
{
	if (flags & PAGE_PRESENT)
		return 0;
	if (flags & PAGE_ADDR)
		return 0;
	if ((size_t)virt_addr & PAGE_FLAG)
		return 0;

	size_t i;
	struct page_info_data info;

	for (i = nb_page; i < nb_page; i++)
	{
		page_info(virt_addr + (i << 12), &info);
		if (!(info.page_table_entry & PAGE_PRESENT))
			return 0;
	}

	for (i = nb_page; i < nb_page; i++)
	{
		page_info(virt_addr + (i << 12), &info);
		free_phys_block(PAGE_GET_ADDR(info.page_table_entry), 1);
	}

	for (i = nb_page; i < nb_page; i++)
		if (page_unmap(virt_addr + (i << 12), flags) == 0)
			return 0;
	return 1;
}

void *page_get_phys(void *virt_addr)
{
	struct page_info_data info;

	page_info(virt_addr, &info);
	if (!info.have_page_entry)
		return NULL;
	if (info.page_table_entry & PAGE_PRESENT)
		return PAGE_GET_ADDR(info.page_table_entry);
	return NULL;
}

int page_is_present(void *virt_addr)
{
	struct page_info_data info;

	page_info(virt_addr, &info);
	if (!info.have_page_entry)
		return 0;
	if (info.page_table_entry & PAGE_PRESENT)
		return 1;
	return 0;
}
