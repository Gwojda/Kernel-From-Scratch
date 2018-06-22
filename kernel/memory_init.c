#include "memory.h"

void memory_init(unsigned long magic, unsigned long addr)
{
	struct multiboot_tag *tag;

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
	{
		printk ("Invalid magic number: 0x%x\n", (unsigned) magic);
		return;
	}
	if (addr & 7)
	{
		printk ("Unaligned mbi: 0x%x\n", addr);
		return;
	}
	for (tag = (struct multiboot_tag *) (addr + 8);
			tag->type != MULTIBOOT_TAG_TYPE_END;
			tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
				+ ((tag->size + 7) & ~7)))
	{
		switch (tag->type)
		{
			case MULTIBOOT_TAG_TYPE_MMAP:
				{
					multiboot_memory_map_t *mmap;
					size_t			tot_free_length = 0;

					for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
							(multiboot_uint8_t *) mmap
							< (multiboot_uint8_t *) tag + tag->size;
							mmap = (multiboot_memory_map_t *)
							((unsigned long) mmap
							 + ((struct multiboot_tag_mmap *) tag)->entry_size))
					{
						printk (" base_addr = 0x%x%x,"
								" length = 0x%x%x, type = 0x%x\n",
								(unsigned) (mmap->addr >> 32),
								(unsigned) (mmap->addr & 0xffffffff),
								(unsigned) (mmap->len >> 32),
								(unsigned) (mmap->len & 0xffffffff),
								(unsigned) mmap->type);
						if (mmap->type == 1)
						{
							tot_free_length += (unsigned) (mmap->len >> 32);
							tot_free_length += (unsigned) (mmap->len & 0xffffffff);
						}
					}
				break;
			}
		}
	}
	tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
			+ ((tag->size + 7) & ~7));
}
