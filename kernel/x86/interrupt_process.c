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

static void proc_load(struct process* proc, struct interupt *data)
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

static struct process *global_old;
static struct process *global_new;
static char proc_switch_context[4096 * 2];
void switch_stack(void *, void *);
void proc_switch_iret(struct interupt);

void proc_switch_p(void)
{
	struct interupt data;
	struct interupt *dataptr;

	if (global_old != NULL)
	{
		if (process_memory_switch(global_old, 0) != 0)
			kern_panic("error switch process: unmap\n");
	}
	proc_load(global_new, &data);
	if (process_memory_switch(global_new, 1) != 0)
		kern_panic("error switch process: map\n");
	if (&global_new->signal.sig_queue.list != global_new->signal.sig_queue.list.next)
		send_signal(global_new);
	if (data.cs != GDT_SEG_KCODE)
	{
		proc_switch_iret(data);
	}
	//we want to switch of stack, let setup the stack
	dataptr = (struct interupt*)(global_new->regs.esp - 0x30);
	dataptr->ds = data.ds;
	dataptr->es = data.es;
	dataptr->fs = data.fs;
	dataptr->gs = data.gs;

	dataptr->edi = data.edi;
	dataptr->esi = data.esi;
	dataptr->ebp = data.ebp;
	dataptr->esp = data.esp;
	dataptr->ebx = data.ebx;
	dataptr->edx = data.edx;
	dataptr->ecx = data.ecx;
	dataptr->eax = data.eax;

	dataptr->int_no = data.int_no;
	dataptr->err_code = data.err_code;

	dataptr->eip = data.eip;
	dataptr->cs = data.cs;
	dataptr->eflags = data.eflags;
	// We dont push useresp and ss to not corupt the stack
	switch_stack(((char*)dataptr), proc_switch_iret);
}

void proc_switch(struct interupt *data, struct process *old, struct process *new)
{
	if (old != NULL)
		proc_save(old, data);
	if (new == NULL)
		new = process_hlt;
	global_old = old;
	global_new = new;
	switch_stack((void *)proc_switch_context + 4096, proc_switch_p);
}
