#include "memory.h"

t_memory	mm_info = 
{
	0,
	NULL,
	0
};

void memory_init(unsigned long magic, unsigned long addr)
{
	struct multiboot_tag *tag;

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		kern_panic("Invalid magic number: 0x%x\n", (unsigned) magic);
	if (addr & 7)
		kern_panic("Unaligned mbi: 0x%x\n", addr);
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
				current_addr = (unsigned) (mmap->addr & 0xffffffff);
				current_addr += (unsigned) (mmap->addr >> 32);
				current_len = (unsigned) (mmap->len & 0xffffffff);
				current_len += (unsigned) (mmap->len >> 32);
				if (mmap->type == 1 && current_addr == NULL)
					mm_info.lowMemorySize = current_len;
				else if (mmap->type == 1 && current_addr == HIGH_MEMORY_BEGIN)
				{
					mm_info.highMemory = current_addr;
					mm_info.highMemorySize = current_len;
				}
			}
			break;
		}
	}
}
