#include "process.h"
void	init();

void	init_process(void)
{
	struct process *w = process_ini_kern(init, (void*)init + 0xC0000000, 1 << 12);
	process_memory_switch(w, 0);
	if ((process_hlt = process_hlt_create()) == NULL)
		kern_panic("Can not setup process\n");
	print_initialize_status("Process", TRUE);
}
