#include "position.h"
#include "page.h"

void	stack_setup(void)
{
	void*	stack_btmp = &stack_top;
	void*	virt_stack_ptr = STACK_END - 4096;

	while (stack_btmp != &stack_bottom && virt_stack_ptr >= STACK_START)
	{
		if (!page_map(KERNEL_GET_REAL(stack_btmp), virt_stack_ptr, PAGE_WRITE | PAGE_PRESENT))
			kern_panic("Stack setup failled\n");
		virt_stack_ptr -= 4096;
		stack_btmp -= 4096;
	}
	if (page_map_at(virt_stack_ptr, PAGE_WRITE | PAGE_PRESENT, (virt_stack_ptr - STACK_START) >> 12) == 0)
		kern_panic("impossible to map");
}
