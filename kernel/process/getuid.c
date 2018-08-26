#include "process.h"
#include "page.h"
#include "GDT.h"
#include "printk.h"

int		getuid(struct process *proc)
{
	return proc->uid;
}
