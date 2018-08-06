#include "phys_mem_management.h"

static void	alloc_phys_block(void *start_phys_addr, size_t size)
{
	while (size)
	{
		mm_bitmap[ACCESS_BITMAP_BY_ADDR(start_phys_addr)] &= ~(1 << (((size_t)start_phys_addr >> 12) % 8));
		start_phys_addr += 4096;
		--size;
	}
}

void	free_phys_block(void *start_phys_addr, size_t size)
{
	while (size)
	{
		mm_bitmap[ACCESS_BITMAP_BY_ADDR(start_phys_addr)] |= 1 << (((size_t)start_phys_addr >> 12) % 8);
		start_phys_addr += 4096;
		--size;
	}
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
			{
				alloc_phys_block((i * 8 + j - following_bits) << 12, following_bits);
				return ((i * 8 + j - following_bits) << 12);
			}
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
