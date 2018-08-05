#include "position.h"
#include "page.h"

void	stack_setup(void)
{
	void*	stack_btmp = &stack_top;
	void*	virt_stack_ptr = STACK_END - 4096;
	void	*phys_addr;
	
	while (stack_btmp != &stack_bottom/* && virt_stack_ptr > STACK_START*/)
	{
		printk(" %p", virt_stack_ptr);
		if (!page_map(KERNEL_GET_REAL(stack_btmp), virt_stack_ptr, PAGE_WRITE | PAGE_PRESENT))
			kern_panic("Stack setup failled\n");
		virt_stack_ptr -= 4096;
		stack_btmp -= 4096;
	}
/*	while (virt_stack_ptr > STACK_START)
	{
		phys_addr = get_phys_block(1);
		if (!page_map(phys_addr, virt_stack_ptr, PAGE_WRITE | PAGE_PRESENT))
			kern_panic("Stack setup failled\n");
		virt_stack_ptr -= 4096;
	}*/
}
