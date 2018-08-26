#include "kernel.h"
#include "errno.h"

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

int page_entry_set(uint32_t *table, unsigned int index, void *ptr, unsigned int flag)
{
	if ((size_t)ptr & PAGE_FLAG)
		return -EINVAL;
	if (flag & PAGE_ADDR)
		return -EINVAL;
	if (index >= 1024)
		return -EINVAL;
	table[index] = ((size_t)ptr) | flag;
	return 0;
}

int page_entry_remove(uint32_t *table, unsigned int index)
{
	return page_entry_set(table, index, 0x00000000, PAGE_NOTHING);
}

int page_entry_set_range(uint32_t *table, unsigned int from, unsigned int to, unsigned flag)
{
	int err = 0;
	unsigned int i;

	for (i = from; i < to; i++)
		if ((err = page_entry_set(table, i, (void*)(i << 12), flag)) != 0)
			break ;
	return err;
}

int access_table_with_physical(void *empty_static_page, void *physical)
{
	void *i = KERNEL_GET_REAL(empty_static_page);
	int err = page_entry_set(kernel_page, (size_t)i >> 12, physical, PAGE_WRITE | PAGE_PRESENT);
	invlpg(empty_static_page);
	return err;
}

int page_unmap(void *virt_addr, unsigned flag)
{
	int err = 0;

	if ((uint32_t)virt_addr & PAGE_FLAG)
		return -EINVAL;

	unsigned int page_directory_index = (size_t)virt_addr >> 22;
	unsigned int page_table_index = ((size_t)virt_addr >> 12) & 0x03FF;

	uint32_t *page_directory = KERNEL_GET_VIRTUAL(page_directory_get());//
	uint32_t *directory_entry = &(page_directory[page_directory_index]);

	if (*directory_entry & PAGE_SIZE)
		return -EINVAL;
	if (!(*directory_entry & PAGE_PRESENT))
		return -EINVAL;

	uint32_t *page_table = page_swap;
	if ((err = access_table_with_physical(page_swap, PAGE_GET_ADDR(*directory_entry))) != 0)
		return err;
	if ((err = page_entry_set(page_table, page_table_index, NULL, PAGE_NOTHING)) != 0)
		return err;
	invlpg(virt_addr);

	unsigned i = 0;
	while (i < 1024)
	{
		if (page_table[i] & PAGE_PRESENT)
			return err;
		i++;
	}

	*directory_entry &= ~PAGE_PRESENT;
	free_phys_block(PAGE_GET_ADDR(*directory_entry), 1);
	*directory_entry = flag;

	page_directory_reset(); // page update

	return err;
}

int page_map(void *phy_addr, void *virt_addr, unsigned int flag)
{
	int err = 0;;

	if ((uint32_t)virt_addr & PAGE_FLAG)
		return -EINVAL;
	if ((uint32_t)phy_addr & PAGE_FLAG)
		return -EINVAL;
	if (flag & PAGE_ADDR)
		return -EINVAL;
	if (!(flag & PAGE_PRESENT))
		return page_unmap(virt_addr, flag);

	unsigned int page_directory_index = (size_t)virt_addr >> 22;
	unsigned int page_table_index = ((size_t)virt_addr >> 12) & 0x03FF;

	uint32_t *page_directory = KERNEL_GET_VIRTUAL(page_directory_get());//
	uint32_t *directory_entry = &(page_directory[page_directory_index]);

	if (*directory_entry & PAGE_SIZE)
		return -EINVAL;
	if (!(*directory_entry & PAGE_PRESENT))
	{
		if (!(*directory_entry = (uint32_t)get_phys_block(1)))
			return -ENOMEM;
		*directory_entry |= PAGE_PRESENT | PAGE_WRITE | PAGE_USER_SUPERVISOR;
		page_directory_reset(); // page update
	}

	uint32_t *page_table = page_swap;
	if ((err = access_table_with_physical(page_swap, PAGE_GET_ADDR(*directory_entry))) != 0)
		return err;
	if ((err = page_entry_set(page_table, page_table_index, phy_addr, flag)) != 0)
		return err;
	invlpg(virt_addr);

	return err;
}

int page_map_range(void *phy_addr, void *virt_addr, unsigned flags, size_t size)
{
	size_t i;
	int err = 0;

	for (i = 0; i < size; i++)
		if ((err = page_map(phy_addr + (i << 12), virt_addr + (i << 12), flags)))
			goto error;
	goto end;
end:
	return err;
error:
	for (; i > 0; i--)
		if (page_unmap(virt_addr + (i << 12), PAGE_NOTHING) == 0)
			continue;
	goto end;
}

int page_map_at(void *virt_addr, unsigned flags, size_t nb_page)
{
	void *new_page;
	int err;

	if (!(flags & PAGE_PRESENT))
		return -EINVAL;
	if (flags & PAGE_ADDR)
		return -EINVAL;
	if ((size_t)virt_addr & PAGE_FLAG)
		return -EINVAL;

	if (!(new_page = get_phys_block(nb_page)))
		return -ENOMEM;
	if ((err = page_map_range(new_page, virt_addr, flags, nb_page)) != 0)
	{
		free_phys_block(new_page, nb_page);
		return err;
	}
	return 0;
}

int page_unmap_at(void *virt_addr, unsigned flags, size_t nb_page)
{
	if (flags & PAGE_PRESENT)
		return -EINVAL;
	if (flags & PAGE_ADDR)
		return -EINVAL;
	if ((size_t)virt_addr & PAGE_FLAG)
		return -EINVAL;

	size_t i;
	struct page_info_data info;
	int err;

	for (i = nb_page; i < nb_page; i++)
	{
		page_info(virt_addr + (i << 12), &info);
		if (!(info.page_table_entry & PAGE_PRESENT))
			return -EINVAL;
	}

	for (i = nb_page; i < nb_page; i++)
	{
		page_info(virt_addr + (i << 12), &info);
		free_phys_block(PAGE_GET_ADDR(info.page_table_entry), 1);
	}

	for (i = nb_page; i < nb_page; i++)
		if ((err = page_unmap(virt_addr + (i << 12), flags)) != 0)
			return err;
	return 0;
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
