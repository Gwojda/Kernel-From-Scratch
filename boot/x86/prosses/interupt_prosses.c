#include "idt.h"
#include "prosses.h"
#include "GDT.h"

void pros_save(struct prosses* pros, struct interupt *data)
{
	pros->regs.ds = data->ds;
	pros->regs.es = data->es;
	pros->regs.fs = data->fs;
	pros->regs.gs = data->gs;

	pros->regs.edi = data->edi;
	pros->regs.esi = data->esi;
	pros->regs.ebp = data->ebp;
	pros->regs.ebx = data->ebx;
	pros->regs.edx = data->edx;
	pros->regs.ecx = data->ecx;
	pros->regs.eax = data->eax;

	pros->regs.eip = data->eip;
	pros->regs.cs = data->cs;
	pros->regs.eflags = data->eflags;
	if (data->cs != GDT_SEG_KCODE)
	{
		pros->regs.esp = data->useresp;
		pros->regs.ss = data->ss;
	}
	else
		pros->regs.esp = data->esp;
}

void pros_load(struct prosses* pros, struct interupt *data)
{
	data->ds = pros->regs.ds;
	data->es = pros->regs.es;
	data->fs = pros->regs.fs;
	data->gs = pros->regs.gs;

	data->edi = pros->regs.edi;
	data->esi = pros->regs.esi;
	data->ebp = pros->regs.ebp;
	data->ebx = pros->regs.ebx;
	data->edx = pros->regs.edx;
	data->ecx = pros->regs.ecx;
	data->eax = pros->regs.eax;

	data->eip = pros->regs.eip;
	data->cs = pros->regs.cs;
	data->eflags = (pros->regs.eflags | 0x200) & 0xFFFFBFFF;
	if (data->cs != GDT_SEG_KCODE) // USERLAND
	{
		data->useresp = pros->regs.esp;
		data->ss = pros->regs.ss;
	}
	else
		data->esp = pros->regs.esp;
}

int pros_switch(struct interupt *data, struct prosses *old, struct prosses *new)
{
	//TODO bad idee becose we can corrupt the stack with user esp and user ss
	//struct interupt *new_data;

	if (old != NULL)
	{
		pros_save(old, data);
		if (prosses_memory_switch(old, 0) != 0)
			return 1;
	}
	if (&new->signal.sig_queue.list != new->signal.sig_queue.list.next)
		send_signal(new);
	if (new == NULL)
		return 3;
	pros_load(new, data);
	if (prosses_memory_switch(new, 1) != 0)
		return 2;
	return 0;
}

