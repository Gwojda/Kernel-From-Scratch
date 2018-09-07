#include "virt_mem_management.h"

# define TEST_SIZE 2048

void	mem_tester(void)
{
	char	*ptr[TEST_SIZE];
	int i = 0;
	int j = 1;

	if (kmalloc(0) != NULL || vmalloc(0) != NULL)
	{
		printk("error ! Null ptr excepted\n");
		return ;
	}
	while (j < TEST_SIZE)
	{
		i = 0;
		while (i < TEST_SIZE)
		{
			if (!(ptr[i] = kmalloc(j)))
				printk("kmalloc failled\n");
			*(ptr[i]) = 1;
			++i;
		}
		i = 0;
		while (i < TEST_SIZE)
		{
			kfree(ptr[i]);
			++i;
		}
		i = 0;
		while (i < TEST_SIZE)
		{
			if (!(ptr[i] = vmalloc(j)))
				printk("vmalloc failled\n");
			(*ptr[i]) = 1;
			++i;
		}
		i = 0;
		while (i < TEST_SIZE)
		{
			vfree(ptr[i]);
			++i;
		}
		i = 0;
		while (i < TEST_SIZE)
		{
			if (!(ptr[i] = kmalloc(j)))
				printk("kmalloc failled\n");
			*(ptr[i]) = 1;
			kfree(ptr[i]);
			++i;
		}
		printk("test %d passed well\n", j);
		++j;
	}

	printk("Mem seems to work well !\n");
}
