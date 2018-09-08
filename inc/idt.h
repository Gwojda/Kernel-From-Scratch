#ifndef IDT_H
# define IDT_H

# include "signal.h"
# include "typedef.h"
# include "io.h"

# define IDT_BASE	0x00000000
# define IDT_SIZE	0xFF

# define IDT_FLAGS_PRESENT	0b10000000
# define IDT_FLAGS_RING0	0b00000000
# define IDT_FLAGS_RING1	0b00100000
# define IDT_FLAGS_RING2	0b01000000
# define IDT_FLAGS_RING3	0b01100000
# define IDT_FLAGS_TYPE_INT32	0b00001110 // not interuptible
# define IDT_FLAGS_TYPE_TRAP32	0b00001111 // interuptible

# define NOT_GATE	(0x0)
# define INT_GATE	(IDT_FLAGS_PRESENT | IDT_FLAGS_TYPE_INT32)
# define INT_GATE_R3	(IDT_FLAGS_PRESENT | IDT_FLAGS_TYPE_INT32 | IDT_FLAGS_RING3)
# define TRAP_GATE	(IDT_FLAGS_PRESENT | IDT_FLAGS_TYPE_TRAP32)
# define TRAP_GATE_R3	(IDT_FLAGS_PRESENT | IDT_FLAGS_TYPE_TRAP32 | IDT_FLAGS_RING3)

struct idtr {
	u16 limite;
	void *base;
} __attribute__ ((packed));

struct idtdesc {
	u16 offset0_15;
	u16 select;
	u8 zero;
	u8 type;
	u16 offset16_31;
} __attribute__ ((packed));

struct interupt
{
	u32 ds, es, fs, gs;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 int_no, err_code;
	u32 eip, cs, eflags, useresp, ss;
};

struct err_code_pagefault
{
	u32 protection : 1;
	u32 write : 1;
	u32 user : 1;
	u32 reserve : 1;
	u32 execute : 1;
};

extern struct idtdesc kidt[IDT_SIZE];

void _asm_irq_end(struct interupt data);

void _asm_irq_0(void);
void _asm_irq_1(void);
void _asm_irq_2(void);
void _asm_irq_3(void);
void _asm_irq_4(void);
void _asm_irq_5(void);
void _asm_irq_6(void);
void _asm_irq_7(void);
void _asm_irq_8(void);
void _asm_irq_9(void);
void _asm_irq_10(void);
void _asm_irq_11(void);
void _asm_irq_12(void);
void _asm_irq_13(void);
void _asm_irq_14(void);
void _asm_irq_15(void);
void _asm_irq_16(void);
void _asm_irq_17(void);
void _asm_irq_18(void);
void _asm_irq_19(void);
void _asm_irq_20(void);
void _asm_irq_21(void);
void _asm_irq_22(void);
void _asm_irq_23(void);
void _asm_irq_24(void);
void _asm_irq_25(void);
void _asm_irq_26(void);
void _asm_irq_27(void);
void _asm_irq_28(void);
void _asm_irq_29(void);
void _asm_irq_30(void);
void _asm_irq_31(void);
void _asm_irq_32(void);
void _asm_irq_33(void);
void _asm_irq_128(void);

void init_idt(void);
void switch_process(struct interupt *data);

#endif
