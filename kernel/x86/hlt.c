#include "process.h"
#include "GDT.h"

struct process	*process_hlt_creat(void)
{
	struct process *proc;

	if ((proc = process_new()) == NULL)
		return NULL;

	proc->regs.eip = (u32)process_hlt_user;
	proc->regs.cs = GDT_SEG_KCODE;
	proc->regs.ss = GDT_SEG_KSTACK;
	proc->regs.ds = GDT_SEG_KDATA;
	proc->regs.es = GDT_SEG_KDATA;
	proc->regs.fs = GDT_SEG_KDATA;
	proc->regs.gs = GDT_SEG_KDATA;
	return proc;
}
