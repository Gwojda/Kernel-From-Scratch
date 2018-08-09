#include "idt.h"
#include "printk.h"
#include "panic.h"
#include "page.h"
#include "vga.h"

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

void irq_clock(struct interupt data)
{
	(void)data;
	//printk("-");
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

//	init_idt_desc(0x08, _asm_syscalls, TRAP_GATE, &kidt[48]);
//	init_idt_desc(0x08, _asm_syscalls, TRAP_GATE, &kidt[128]); //48

	kidtr.limite = IDT_SIZE * sizeof(struct idtdesc) - 1;
	kidtr.base = &kidt;

	/* Load the IDTR registry */
	asm("lidt (kidtr)");
	outb(0x21 , 0xFC);
	asm volatile("sti");
	print_initialize_status("IDT", TRUE);
}
