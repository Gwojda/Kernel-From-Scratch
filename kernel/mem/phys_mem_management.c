#include "phys_mem_management.h"
#include "page.h"
#include "lib.h"

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
	if (ACCESS_BITMAP_BY_ADDR(start_phys_addr) >= MAX_RAM_PAGE / 8)
	{
		printk("try to free unvalaible physical block\n");
		return ;
	}
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
				alloc_phys_block((void *)((i * 8 + j - following_bits) << 12), following_bits);
				return ((void *)((i * 8 + j - following_bits) << 12));
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

int		phys_bzero(void *addr, size_t nb_page)
{
	while (nb_page > 0)
	{
		access_table_with_physical(page_swap, addr);
		bzero(page_swap, 1 << 12);
		addr += 1 << 12;
		nb_page--;
	}
	return 0;
}
