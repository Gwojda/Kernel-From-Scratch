#include "idt.h"
#include "process.h"
#include "GDT.h"

void proc_save(struct process* proc, struct interupt *data)
{
	proc->regs.ds = data->ds;
	proc->regs.es = data->es;
	proc->regs.fs = data->fs;
	proc->regs.gs = data->gs;

	proc->regs.edi = data->edi;
	proc->regs.esi = data->esi;
	proc->regs.ebp = data->ebp;
	proc->regs.ebx = data->ebx;
	proc->regs.edx = data->edx;
	proc->regs.ecx = data->ecx;
	proc->regs.eax = data->eax;

	proc->regs.eip = data->eip;
	proc->regs.cs = data->cs;
	proc->regs.eflags = data->eflags;
	if (data->cs != GDT_SEG_KCODE)
	{
		proc->regs.esp = data->useresp;
		proc->regs.ss = data->ss;
	}
	else
		proc->regs.esp = data->esp;
}

void proc_load(struct process* proc, struct interupt *data)
{
	data->ds = proc->regs.ds;
	data->es = proc->regs.es;
	data->fs = proc->regs.fs;
	data->gs = proc->regs.gs;

	data->edi = proc->regs.edi;
	data->esi = proc->regs.esi;
	data->ebp = proc->regs.ebp;
	data->ebx = proc->regs.ebx;
	data->edx = proc->regs.edx;
	data->ecx = proc->regs.ecx;
	data->eax = proc->regs.eax;

	data->eip = proc->regs.eip;
	data->cs = proc->regs.cs;
	data->eflags = (proc->regs.eflags | 0x200) & 0xFFFFBFFF;
	if (data->cs != GDT_SEG_KCODE) // USERLAND
	{
		data->useresp = proc->regs.esp;
		data->ss = proc->regs.ss;
	}
	else
		data->esp = proc->regs.esp;
}

int proc_switch(struct interupt *data, struct process *old, struct process *new)
{
	//TODO bad idee because we can corrupt the stack with user esp and user ss
	//struct interupt *new_data;

	if (old != NULL)
	{
		proc_save(old, data);
		if (process_memory_switch(old, 0) != 0)
			return 1;
	}
	if (&new->signal.sig_queue.list != new->signal.sig_queue.list.next)
		send_signal(new);
	if (new == NULL)
		return 3;
	proc_load(new, data);
	if (process_memory_switch(new, 1) != 0)
		return 2;
	return 0;
}
