#include "backtrace.h"
#include "printk.h"
#include "page.h"

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
		if (page_is_present(ebp + 1) == 0)
			goto bad_stack;
		if (page_is_present((char*)(ebp + 1) + sizeof(ebp) - 1) == 0)
			goto bad_stack;
		eip = (void*)ebp[1];
		printk("  %p\n", eip);
		if (eip == NULL)
			break ;
		if (page_is_present(ebp) == 0)
			goto bad_stack;
		if (page_is_present((char*)ebp + sizeof(ebp) - 1) == 0)
			goto bad_stack;
		ebp = (size_t*)*ebp;
		max--;
	}
	return;
bad_stack:
	printk("  stack corrupt\n");
}
