#include "idt.h"
#include "printk.h"
#include "panic.h"
#include "page.h"
#include "vga.h"
#include "backtrace.h"
#include "GDT.h"

struct idtr kidtr;
struct idtdesc kidt[IDT_SIZE];

void init_idt_desc(u16 select, void (*offset)(), u16 type, struct idtdesc *desc)
{
	desc->offset0_15 = ((size_t)offset & 0xffff);
	desc->select = select;
	desc->zero = 0;
	desc->type = type;
	desc->offset16_31 = ((size_t)offset & 0xffff0000) >> 16;
	return;
}

static void initialize_pic()
{
	/* ICW1 - begin initialization */
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	/* ICW2 - remap offset address of idt_table */
	/*
	 * In x86 protected mode, we have to remap the PICs beyond 0x20 because
	 * Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	 */
	outb(0x21, 0x20);
	outb(0xA1, 0x28);

	/* ICW3 - setup cascading */
	outb(0x21, 0x00);
	outb(0xA1, 0x00);

	/* ICW4 - environment info */
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	/* Initialization finished */

	/* mask interrupts */
	outb(0x21 , 0xff);
	outb(0xA1 , 0xff);
}

#include "prosses.h"

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

void pros_cmp(struct prosses* pros, struct interupt *data)
{
	printk("MEM\n");
	printk("ds %p %p\n", pros->regs.ds, data->ds);
	printk("es %p %p\n", pros->regs.es, data->es);
	printk("fs %p %p\n", pros->regs.fs, data->fs);
	printk("gs %p %p\n", pros->regs.gs, data->gs);

	printk("edi %p %p\n", pros->regs.edi, data->edi);
	printk("esi %p %p\n", pros->regs.esi, data->esi);
	printk("ebp %p %p\n", pros->regs.ebp, data->ebp);
	printk("ebx %p %p\n", pros->regs.ebx, data->ebx);
	printk("edx %p %p\n", pros->regs.edx, data->edx);
	printk("ecx %p %p\n", pros->regs.ecx, data->ecx);
	printk("eax %p %p\n", pros->regs.eax, data->eax);

	printk("eip %p %p\n", pros->regs.eip, data->eip);
	printk("cs %p %p\n", pros->regs.cs, data->cs);
	printk("eflags %p %p\n", pros->regs.eflags, data->eflags);
	if (data->cs != GDT_SEG_KCODE)
	{
		printk("esp %p %p\n", pros->regs.esp, data->useresp);
		printk("ss %p %p\n", pros->regs.ss, data->ss);
	}
	else
		printk("esp %p %p\n", pros->regs.esp, data->esp);
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
	struct interupt *new_data;

	if (old != NULL)
	{
		pros_save(old, data);
		if (prosses_memory_switch(old, 0) != 0)
			return 1;
	}
	if (new == NULL)
		return 3;
	pros_load(new, new_data);
	if (prosses_memory_switch(new, 1) != 0)
		return 2;
	return 0;
}

void irq_clock(struct interupt data)
{
	outb(0x20, 0x20);
	return ;// REMOVE FOR MULTI PROSSES
	struct prosses *old = current;

	if (current == NULL)
		current = &prosses_list;

	current = list_entry(current->plist.next, struct prosses, plist);
	if (&current->plist == &prosses_list)
	{
		current = list_first_entry(&prosses_list, struct prosses, plist);
		if (&current->plist == &prosses_list)
			current = NULL;
	}

	int i;
	if ((i = pros_switch(&data, old, current)) != 0)
		kern_panic("Fail to switch prosses %d\n", i);

	outb(0x20, 0x20);
}

struct stream keybord_stream;

void irq_keybord(struct interupt data)
{
	char get_key;

	(void)data;
	if (inb(0x64) & 1)
	{
		get_key = inb(0x60);
		stream_write(&keybord_stream, &get_key, 1);
	}
	outb(0x20, 0x20);
}

void irq_pagefault(struct interupt data)
{
	const struct err_code_pagefault *err = (void*)&(data.err_code);
	void *addr;

	asm("mov %%cr2, %0" : "=r"(addr));
	printk("Page fault %p: ", addr);
	if (err->write)
		printk("write ");
	else
		printk("read ");
	if (err->protection)
		printk("cause protection violation ");
	else
		printk("to not present page ");
	if (err->user)
		printk("in user mode\n");
	else
		printk("in supervisor mode\n");
	if (err->reserve)
		printk("  reserve byte set in page directory / entry\n");
	if (err->execute)
		printk("  execute not execute memory\n");
	page_info_display(addr);
	backtrace((size_t*)data.ebp, 5);
	kern_panic("");
}

void irq_divisionbyzero(struct interupt data)
{
	(void)data;
	kern_panic("division by zero\n");
}

void irq_doublefault(struct interupt data)
{
	(void)data;
	kern_panic("Double fault\n");
}

void irq_invalidopcode(struct interupt data)
{
	(void)data;
	kern_panic("Invalid Opcode\n");
}

void irq_overflow(struct interupt data)
{
	(void)data;
	kern_panic("Over flow\n");
}

void irq_invalidetaskstatesegment(struct interupt data)
{
	(void)data;
	kern_panic("Invalide task state segment\n");
}

void irq_segmentnotpresent(struct interupt data)
{
	(void)data;
	kern_panic("Segment not present\n");
}

void irq_stackfault(struct interupt data)
{
	(void)data;
	kern_panic("Stack fault\n");
}

void irq_generalprotection(struct interupt data)
{
	(void)data;
	printk("eip %p\n", data.eip);
	printk("gs %p\n", data.gs);
	printk("fs %p\n", data.fs);
	printk("es %p\n", data.es);
	printk("ds %p\n", data.ds);
	kern_panic("General protection fault %p\n", data.err_code);
}

void irq_mathfault(struct interupt data)
{
	(void)data;
	kern_panic("Math fault\n");
}

void irq_machinecheck(struct interupt data)
{
	(void)data;
	kern_panic("Machine check\n");
}

void usless_function(struct interupt data)
{
	(void)data;
}

void init_idt(void)
{
	initialize_pic();
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
	outb(0x21 , 0xFC);
	print_initialize_status("IDT", TRUE);
}
