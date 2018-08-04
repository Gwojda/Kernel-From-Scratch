#include "virt_mem_management.h"

void	init_free_vm(void)
{
	free_vm.vm_start = 0x00000000;
	free_vm.vm_end = 0xFFFFFFFF;
	INIT_LIST_HEAD(&free_vm.list);
}

