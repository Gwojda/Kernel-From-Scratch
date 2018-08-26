#include "virt_mem_management.h"

void	mem_tester(void)
{
	char	ptr[4096];
	int i = 0;
	int j = 0;

	while (j < 128)
	{
		i = 0;
		while (i < 4096)
		{
			ptr[i] = kmalloc(j);
			++i;
		}
		i = 0;
		while (i < 4096)
		{
			kfree(ptr[i]);
			++i;
		}
		i = 0;
		while (i < 4096)
		{
			ptr[i] = vmalloc(j);
			++i;
		}
		i = 0;
		while (i < 4096)
		{
			vfree(ptr[i]);
			++i;
		}

		while (i < 4096)
		{
			ptr[i] = kmalloc(j);
			kfree(ptr[i]);
			++i;
		}
		++j;
	}
}
