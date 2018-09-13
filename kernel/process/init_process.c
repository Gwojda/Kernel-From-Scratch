#include "process.h"
#include "tty.h"

void	init();

void	init_process(void)
{
	struct process *w;
	if (!(w = process_ini_kern((u32 *)init, (void*)init + 0xC0000000, 1 << 12)))
		return ;
	process_memory_switch(w, 0);
	if ((process_hlt = process_hlt_create()) == NULL)
		kern_panic("Can not setup process\n");
	print_initialize_status("Process", TRUE);
}
