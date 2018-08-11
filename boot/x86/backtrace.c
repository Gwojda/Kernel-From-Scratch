#include "backtrace.h"
#include "printk.h"

void	backtrace_cur(unsigned long max)
{
	size_t *ebp;

	__asm__ volatile ("movl %%ebp, %[r]" :  [r] "=r" (ebp));
	backtrace(ebp, max);
}

void	backtrace(size_t *ebp, unsigned long max)
{
	void *eip;

	printk("backtrace\n");
	while (max > 0 && ebp != NULL)
	{
		eip = (void*)ebp[1];
		printk("  %p\n", eip);
		if (eip == NULL)
			break ;
		ebp = (size_t*)*ebp;
		max--;
	}
}
