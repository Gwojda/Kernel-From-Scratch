#include "phys_mem_management.h"

static void	alloc_phys_block(void *start_phys_addr, size_t pages_nb)
{
	while (pages_nb / 8)
	{
		mm_bitmap[ACCESS_BITMAP_BY_ADDR(start_phys_addr)] = 0;
		start_phys_addr += 8 * 4096;
		pages_nb -= 8;
	}
	while (pages_nb)
	{
		mm_bitmap[ACCESS_BITMAP_BY_ADDR(start_phys_addr)] &= ~(1 << (pages_nb - 1));
		start_phys_addr += 4096;
		--pages_nb;
	}
}

void	free_phys_block(void *start_phys_addr, size_t pages_nb)
{
	while (pages_nb / 8)
	{
		mm_bitmap[ACCESS_BITMAP_BY_ADDR(start_phys_addr)] = -1;
		start_phys_addr += 8 * 4096;
		pages_nb -= 8;
	}
	while (pages_nb)
	{
		mm_bitmap[ACCESS_BITMAP_BY_ADDR(start_phys_addr)] |= 1 << pages_nb;
		start_phys_addr += 4096;
		--pages_nb;
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
