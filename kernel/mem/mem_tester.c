#include "virt_mem_management.h"
#include "lib.h"

# define TEST_SIZE 2048

void	mem_tester(void)
{
	char	*ptr[TEST_SIZE];
	size_t i = 0;
	size_t j = 1;

	if (kmalloc(0) != NULL || vmalloc(0) != NULL)
	{
		printk("error ! Null ptr excepted\n");
		return ;
	}
	printk("==================================\n");
//	error tester
	while (j < TEST_SIZE)
	{
		i = 0;
		bzero(ptr, TEST_SIZE * sizeof(char *));
		while (i < TEST_SIZE / 2)
		{
			if (!(ptr[i] = kmalloc(j)))
			{
				printk("not enouth mem 1\n");
				break ;
			}
			for (size_t k = 0; k < j; k++)
				ptr[i][k] = 1;
			++i;
		}
		i = 0;
		while (i < TEST_SIZE / 2)
		{
			if (ptr[i])
				kfree(ptr[i]);
			++i;
		}
		bzero(ptr, TEST_SIZE * sizeof(char *));
		i = 0;
		while (i < TEST_SIZE / 2)
		{
			if (!(ptr[i] = vmalloc(j)))
			{
				printk("not enouth mem 2\n");
				break ;
			}
			for (size_t k = 0; k < j; k++)
				ptr[i][k] = 1;
			++i;
		}
		i = 0;
		while (i < TEST_SIZE / 2)
		{
			if (ptr[i])
				vfree(ptr[i]);
			++i;
		}
		i = 0;
		bzero(ptr, TEST_SIZE * sizeof(char *));
		while (i < TEST_SIZE / 2)
		{
			if (!(ptr[i] = kmalloc(j)))
			{
				printk("not enouth mem 3\n");
				break ;
			}
			for (size_t k = 0; k < j; k++)
				ptr[i][k] = 1;
			kfree(ptr[i]);
			++i;
		}
		bzero(ptr, TEST_SIZE * sizeof(char *));

		 // augmenter taille de la heap pour activer ce test
		i = 0;
		while (i < TEST_SIZE / 2)
		{
			if (!(ptr[i] = kmalloc(j * 4096)))
			{
				printk("not enouth mem 4\n");
				break ;
			}
			for (size_t k = 0; k < j; k++)
				ptr[i][k] = 1;
			kfree(ptr[i]);
			++i;
		}
		bzero(ptr, TEST_SIZE * sizeof(char *));
		i = 0;
		while (i < TEST_SIZE / 2)
		{
			if (!(ptr[i] = vmalloc(j * 4096)))
			{
				printk("not enouth mem 5\n");
				break ;
			}
			for (size_t k = 0; k < j; k++)
				ptr[i][k] = 1;
			vfree(ptr[i]);
			++i;
		}
		printk("test %d passed well\n", j);
		++j;
	}


	printk("Mem seems to work well !\n");
}
