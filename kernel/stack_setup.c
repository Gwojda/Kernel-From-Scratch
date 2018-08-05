#include "position.h"
#include "page.h"

void	stack_setup(void)
{
	void*	stack_btmp = &stack_top;
	void*	virt_stack_ptr = STACK_END - 4096;
	void	*phys_addr;
	
	while (stack_btmp != &stack_bottom && virt_stack_ptr >= STACK_START)
	{
		if (!page_map(KERNEL_GET_REAL(stack_btmp), virt_stack_ptr, PAGE_WRITE | PAGE_PRESENT))
			kern_panic("Stack setup failled\n");
		virt_stack_ptr -= 4096;
		stack_btmp -= 4096;
	}
	if (page_map_at(virt_stack_ptr, PAGE_WRITE | PAGE_PRESENT, (virt_stack_ptr - STACK_START) >> 12) == 0)
		kern_panic("imposible to map");
	page_unmap_at(virt_stack_ptr, 0, (virt_stack_ptr - STACK_START) >> 12);
	/*while (virt_stack_ptr >= STACK_START)
	{
		if (!(phys_addr = get_phys_block(1)))
			kern_panic("Stack setup failled no memory left\n");
		if (!page_map(phys_addr, virt_stack_ptr, PAGE_WRITE | PAGE_PRESENT))
			kern_panic("Stack setup failled page_map\n");
		virt_stack_ptr -= 4096;
	}*/
}
