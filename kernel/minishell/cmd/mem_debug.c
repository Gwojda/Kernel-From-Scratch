#include "shell.h"
#include "page.h"

void mem_debug(void)
{
	printk("\n");
	page_info_display_tab();
}
