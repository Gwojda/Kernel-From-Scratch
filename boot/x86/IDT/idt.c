#include "idt.h"
#include "tty.h"
#include "printk.h"
#include "panic.h"
#include "page.h"
#include "vga.h"
#include "backtrace.h"
#include "process.h"
#include "GDT.h"
#include "pic.h"

struct idtr kidtr;
struct idtdesc kidt[IDT_SIZE];

#define SIGNAL_INFO_DEFAULT 0
#define SIGNAL_INFO_CONTINUE 1

int	page_fault_handler(struct interupt *data)
{
	void *addr;

	(void)data;
	if (!current || current->state == ZOMBIE)
		return SIGNAL_INFO_DEFAULT;
	asm("mov %%cr2, %0" : "=r"(addr));
	printk("Page fault %p:\n", addr);
	return SIGNAL_INFO_DEFAULT;
}

struct
{
	int number;
	char *name;
	int signal;
	int error;
	int (*catch)(struct interupt *data);
} signal_info[] = {
	{0,  "Divide by zero",               SIGFPE,  1, NULL},
	{3,  "Break point",                  SIGTRAP, 0, NULL},
	{4,  "Overflow",                     0,       0, NULL},
	{5,  "Bound Range Exceeded",         0,       0, NULL},
	{6,  "Invalide Opcode",              SIGILL,  1, NULL},
	//{7,  "Device Not Available",       0},//
	{8,  "Double Fault",                 -1,      1, NULL},
	{9,  "Coprocessor Segment overrun",  0,       0, NULL},
	{10, "Invalide TSS",                 -1,      1, NULL},
	{11, "Segement not present",         -1,      1, NULL},
	{12, "Stack Segment Fault",          SIGBUS,  1, NULL},
	{13, "General Protection",           SIGSEGV, 1, NULL},
	{14, "Page Fault",                   SIGSEGV, 1, page_fault_handler},
	{16, "x87 Floting point exception",  SIGFPE,  1, NULL},
	{17, "Alignement check",             SIGSEGV, 1, NULL},
	{18, "Machine check",                -1,      1, NULL},
	{19, "SIMD Floting point exception", SIGFPE,  1, NULL},
	//{1,  "Debug",                      0},
	//{2,  "Not maskable Interrupt",     },
	//{20, "Virtualization Exception",   },
	//{30, "Security check",             },
	{-1, NULL, 0, 0, NULL}
};

void init_idt_desc(u16 select, void (*offset)(), u16 type, struct idtdesc *desc)
{
	desc->offset0_15 = ((size_t)offset & 0xffff);
	desc->select = select;
	desc->zero = 0;
	desc->type = type;
	desc->offset16_31 = ((size_t)offset & 0xffff0000) >> 16;
	return;
}

void switch_process(struct interupt *data)
{
	int more_one_process = 0;
	struct process *cur_proc = current;
	struct process *prev_proc;

	prev_proc = cur_proc;
select:
	if (cur_proc == NULL)
		cur_proc = (struct process *)&process_list;
	cur_proc = list_entry(cur_proc->plist.next, struct process, plist);
	if (&cur_proc->plist == &process_list)
	{
		cur_proc = list_first_entry(&process_list, struct process, plist);
		if (&cur_proc->plist == &process_list)
			cur_proc = NULL;
	}

	if (cur_proc && cur_proc->state != RUN && &cur_proc->signal.sig_queue.list == cur_proc->signal.sig_queue.list.next)
	{
		if (cur_proc->state != ZOMBIE)
			more_one_process = 1;
		if (cur_proc == prev_proc)
			cur_proc = NULL;
		else
			goto select;
	}

	if (more_one_process == 0 && cur_proc == NULL)
		kern_panic("No more task\n");
	prev_proc = current;
	current = cur_proc;
	proc_switch(data, prev_proc, current);
}

void irq_clock(struct interupt data)
{
	pic_end_of_interupt(data.int_no);
	switch_process(&data);
}

void irq_keybord(struct interupt data)
{
	char get_key;

	(void)data;
	if (inb(0x64) & 1)
	{
		get_key = key_layout(inb(0x60));
		tty_input_char(current_tty, get_key);
	}
	pic_end_of_interupt(data.int_no);
}

void irq_general(struct interupt data)
{
	int i;
	int err = 0;
	typeof(*signal_info) *s;

	i = 0;
	while (signal_info[i].name)
	{
		s = signal_info + i;
		if ((u32)s->number == data.int_no)
		{
			if (s->catch)
			{
				if (s->catch(&data) == SIGNAL_INFO_CONTINUE)
					return ;
			}
			if (s->signal >= 0 && data.cs != GDT_SEG_KCODE && current && current->state != ZOMBIE)
			{
/*				if (s->signal == 0)
					// zero if for ignore
					return;*/
				if ((err = add_signal(s->signal, current, SIG_HARD)) != 0)
					goto kill_process;
				switch_process(&data);
				//proc_switch(&data, current, current);
				// signal switch
				return;
			}
			if (s->error)
				goto kill_process;
			return;
		}
		i++;
	}
	return;
kill_process:
	printk("Kernel error terminate process %d error code %d\n", s->signal, err);
	if (!current || current->state == ZOMBIE)
	{
		kern_panic("unexpected irq\n");
	}
	process_die(current);
	switch_process(&data);
}

void usless_function(struct interupt data)
{
	(void)data;
}

void init_idt(void)
{
	/* Init irq */
	int i;
	for (i = 0; i < IDT_SIZE; i++)
		init_idt_desc(0x08, usless_function, INT_GATE, &kidt[i]);

	/* Vectors  0 -> 31 are for exceptions */
	init_idt_desc(0x08, _asm_irq_0,  INT_GATE, &kidt[ 0]);
	init_idt_desc(0x08, _asm_irq_1,  INT_GATE, &kidt[ 1]);
	init_idt_desc(0x08, _asm_irq_2,  INT_GATE, &kidt[ 2]);
	init_idt_desc(0x08, _asm_irq_3,  INT_GATE, &kidt[ 3]);
	init_idt_desc(0x08, _asm_irq_4,  INT_GATE, &kidt[ 4]);
	init_idt_desc(0x08, _asm_irq_5,  INT_GATE, &kidt[ 5]);
	init_idt_desc(0x08, _asm_irq_6,  INT_GATE, &kidt[ 6]);
	init_idt_desc(0x08, _asm_irq_7,  INT_GATE, &kidt[ 7]);
	init_idt_desc(0x08, _asm_irq_8,  INT_GATE, &kidt[ 8]);
	init_idt_desc(0x08, _asm_irq_9,  INT_GATE, &kidt[ 9]);
	init_idt_desc(0x08, _asm_irq_10, INT_GATE, &kidt[10]);
	init_idt_desc(0x08, _asm_irq_11, INT_GATE, &kidt[11]);
	init_idt_desc(0x08, _asm_irq_12, INT_GATE, &kidt[12]);
	init_idt_desc(0x08, _asm_irq_13, INT_GATE, &kidt[13]);
	init_idt_desc(0x08, _asm_irq_14, INT_GATE, &kidt[14]);
	init_idt_desc(0x08, _asm_irq_15, INT_GATE, &kidt[15]);
	init_idt_desc(0x08, _asm_irq_16, INT_GATE, &kidt[16]);
	init_idt_desc(0x08, _asm_irq_17, INT_GATE, &kidt[17]);
	init_idt_desc(0x08, _asm_irq_18, INT_GATE, &kidt[18]);
	init_idt_desc(0x08, _asm_irq_19, INT_GATE, &kidt[19]);
	init_idt_desc(0x08, _asm_irq_20, INT_GATE, &kidt[20]);
	init_idt_desc(0x08, _asm_irq_21, INT_GATE, &kidt[21]);
	init_idt_desc(0x08, _asm_irq_22, INT_GATE, &kidt[22]);
	init_idt_desc(0x08, _asm_irq_23, INT_GATE, &kidt[23]);
	init_idt_desc(0x08, _asm_irq_24, INT_GATE, &kidt[24]);
	init_idt_desc(0x08, _asm_irq_25, INT_GATE, &kidt[25]);
	init_idt_desc(0x08, _asm_irq_26, INT_GATE, &kidt[26]);
	init_idt_desc(0x08, _asm_irq_27, INT_GATE, &kidt[27]);
	init_idt_desc(0x08, _asm_irq_28, INT_GATE, &kidt[28]);
	init_idt_desc(0x08, _asm_irq_29, INT_GATE, &kidt[29]);
	init_idt_desc(0x08, _asm_irq_30, INT_GATE, &kidt[30]);
	init_idt_desc(0x08, _asm_irq_31, INT_GATE, &kidt[31]);
	init_idt_desc(0x08, _asm_irq_32, INT_GATE, &kidt[32]);
	init_idt_desc(0x08, _asm_irq_33, INT_GATE, &kidt[33]);

	init_idt_desc(0x08, _asm_irq_128, INT_GATE_R3, &kidt[128]);

	kidtr.limite = IDT_SIZE * sizeof(struct idtdesc) - 1;
	kidtr.base = &kidt;

	/* Load the IDTR registry */
	asm("lidt (kidtr)");
	pic_initialize();
	pic_interupt_mask(~(1 << PIC_INT_KEYBORD | 1 << PIC_INT_ISA_TIMER));
	print_initialize_status("IDT", TRUE);
}
