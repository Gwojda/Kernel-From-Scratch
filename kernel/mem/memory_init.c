#include "page.h"
#include "position.h"

static void	reset_mm_bitmap(void)
{
	size_t i = 0;

	while (i < MAX_RAM_PAGE / 8)
	{
		mm_bitmap[i] = 0;
		++i;
	}
}

static void	init_mm_bitmap(void *start, size_t len)
{
	size_t	page = (size_t)start >> 12;

	len = len >> 12;
	while (len % 8)
	{
		mm_bitmap[ACCESS_BITMAP_BY_ADDR(page << 12)] |= 1 << (page % 8);
		page += 1;
		--len;
	}
	while (len)
	{
		mm_bitmap[ACCESS_BITMAP_BY_ADDR(page << 12)] = -1;
		page += 8;
		len -= 8;
	}
}

void		memory_init_grub(unsigned long magic, unsigned long addr)
{
	struct multiboot_tag *tag;

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		kern_panic("Invalid magic number: 0x%x\n", (unsigned) magic);
	if (addr & 7)
		kern_panic("Unaligned mbi: 0x%x\n", addr);
	reset_mm_bitmap();
	for (tag = (struct multiboot_tag *) (addr + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
	{
		if (tag->type ==  MULTIBOOT_TAG_TYPE_MMAP)
		{
			multiboot_memory_map_t *mmap;
			void			*current_addr = NULL;
			size_t			current_len = 0;

			for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
				(multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size;
				mmap = (multiboot_memory_map_t *)((unsigned long) mmap
				+ ((struct multiboot_tag_mmap *)tag)->entry_size))
			{
				current_addr = (void*)((size_t)mmap->addr & 0xffffffff);
				current_addr += (size_t)(mmap->addr >> 32);
				current_len = (size_t)(mmap->len & 0xffffffff);
				current_len += (size_t)(mmap->len >> 32);
				if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
					init_mm_bitmap(current_addr, current_len);
			}
			break;
		}
	}
}

static void	memory_init_reserv_kernel(void)
{
	unsigned i;

	for (i = (size_t)KERNEL_REAL_START >> 12; i < (size_t)PAGE_ALIGN(KERNEL_REAL_END) >> 12; i++)
		mm_bitmap[ACCESS_BITMAP_BY_ADDR(i << 12)] &= ~(1 << (i % 8));
}

void		memory_init(unsigned long magic, unsigned long addr)
{
	memory_init_grub(magic, addr);
	memory_init_reserv_kernel();
}
