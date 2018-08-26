#include "kernel.h"

int page_info(void *virt_addr, struct page_info_data *ret)
{
	int err = 0;

	ret->target = virt_addr;
	virt_addr = (void *)(((size_t)virt_addr >> 12) << 12);
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
		return err;

	uint32_t *page_table = page_swap;
	if ((err = access_table_with_physical(page_swap, PAGE_GET_ADDR(*directory_entry))))
		return err;

	ret->have_page_entry = 1;
	ret->page_table = page_table;
	ret->page_table_entry = page_table[page_table_index];
	return err;
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
		return 0;
	}
	printk("  page_entry %p\n", ret->page_table);
	printk("  page_table_entry %p\n", ret->page_table_entry);
	printk("   addr %p\n", PAGE_GET_ADDR(ret->page_table_entry));
	printk("   flag %b\n", ret->page_table_entry & PAGE_FLAG);
	for (i = 0; (1 << i) & PAGE_FLAG; i++)
		if ((1 << i) & (size_t)ret->page_table_entry)
			printk("    %s(%d) -> %s\n", flags_info[i].mini, i, flags_info[i].desc);
	return 0;
}

int page_info_display(void *virt_addr)
{
	struct page_info_data info_data;

	page_info(virt_addr, &info_data);
	return page_info_data_display(&info_data);
}

void page_info_display_tab(void)
{
	int i, j;
	struct page_info_data info_data;

	printk("memory dump");
	for (i = 0; i < 1024; i++)
	{
		for (j = 0; j < 1024; j++)
		{
			page_info((void *)((i << 22) + (j << 12)), &info_data);
			if (!info_data.have_page_entry)
				break;
			if (j == 0)
				printk("\n%x ->", i << 10);
			if (info_data.page_table_entry & PAGE_PRESENT)
			{
				printk("  %x", j);
				if (info_data.page_table_entry & PAGE_WRITE)
					printk("w");
				else
					printk(" ");
				if (info_data.page_table_entry & PAGE_USER_SUPERVISOR)
					printk("u");
				else
					printk(" ");
			}
		}
	}
	printk("\n");
}
