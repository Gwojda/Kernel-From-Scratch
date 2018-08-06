#include "idt.h"
#include "printk.h"

struct idtr kidtr;
struct idtdesc kidt[IDT_SIZE];

void init_idt_desc(u16 select, void (*offset)(void), u16 type, struct idtdesc *desc)
{
	desc->offset0_15 = ((size_t)offset & 0xffff);
	desc->select = select;
	desc->zero = 0;
	desc->type = type;
	desc->offset16_31 = ((size_t)offset & 0xffff0000) >> 16;
	return;
}

void isr_default_int(void)
{
	printk("*");
}

void isr_page_fault(void)
{
	int i;
	printk("%p", &i);
}

void isr_clock_int(void)
{
	printk("-");
}

void isr_keybord_int(void)
{
	int status = inb(0x64);
	int data = inb(0x60);
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

void init_idt(void)
{
	initialize_pic();
	/* Init irq */
	int i;
	for (i = 0; i < IDT_SIZE; i++)
		init_idt_desc(0x08, _asm_default_int, INT_GATE, &kidt[i]);
//	init_idt_desc(0x08, _asm_default_int, INT_GATE, &kidt[0xd]);

	/* Vectors  0 -> 31 are for exceptions */
//	init_idt_desc(0x08, _asm_exc_DE, INT_GATE, &kidt[0x00]);
//	init_idt_desc(0x08, _asm_exc_DB, INT_GATE, &kidt[0x01]);
//	init_idt_desc(0x08, _asm_exc_  , INT_GATE, &kidt[0x02]);
//	init_idt_desc(0x08, _asm_exc_BP, INT_GATE, &kidt[0x03]);
//	init_idt_desc(0x08, _asm_exc_OF, INT_GATE, &kidt[0x04]);
//	init_idt_desc(0x08, _asm_exc_BR, INT_GATE, &kidt[0x05]);
//	init_idt_desc(0x08, _asm_exc_UD, INT_GATE, &kidt[0x06]);
//	init_idt_desc(0x08, _asm_exc_NM, INT_GATE, &kidt[0x07]);
//	init_idt_desc(0x08, _asm_exc_DF, INT_GATE, &kidt[0x08]);
//	init_idt_desc(0x08, _asm_exc_  , INT_GATE, &kidt[0x09]);
//	init_idt_desc(0x08, _asm_exc_TS, INT_GATE, &kidt[0x0a]);
//	init_idt_desc(0x08, _asm_exc_NP, INT_GATE, &kidt[0x0b]);
//	init_idt_desc(0x08, _asm_exc_SS, INT_GATE, &kidt[0x0c]);
	init_idt_desc(0x08, _asm_page_fault, INT_GATE, &kidt[0x0d]);
//	init_idt_desc(0x08, _asm_page_fault, INT_GATE, &kidt[0x0e]);
//	init_idt_desc(0x08, _asm_exc_  , INT_GATE, &kidt[0x0f]);
//	init_idt_desc(0x08, _asm_exc_MF, INT_GATE, &kidt[0x10]);
//	init_idt_desc(0x08, _asm_exc_AC, INT_GATE, &kidt[0x11]);
//	init_idt_desc(0x08, _asm_exc_MC, INT_GATE, &kidt[0x12]);
//	init_idt_desc(0x08, _asm_exc_XF, INT_GATE, &kidt[0x13]);

//	init_idt_desc(0x08, _asm_irq_clock, INT_GATE, &kidt[32]);
	init_idt_desc(0x08, _asm_irq_clock, INT_GATE, &kidt[32]);
	init_idt_desc(0x08, _asm_irq_keybord, INT_GATE, &kidt[33]);

//	init_idt_desc(0x08, _asm_syscalls, TRAP_GATE, &kidt[48]);
//	init_idt_desc(0x08, _asm_syscalls, TRAP_GATE, &kidt[128]); //48

	kidtr.limite = IDT_SIZE * sizeof(struct idtdesc) - 1;
	kidtr.base = &kidt;
	printk("idt %p %d\n", kidtr.base, kidtr.limite);

	/* Load the IDTR registry */
	asm("lidt (kidtr)");
	outb(0x21 , 0xFc);
}
