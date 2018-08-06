#ifndef IDT_H
# define IDT_H

# include "typedef.h"

# define IDT_BASE	0x00000000
# define IDT_SIZE	0xFF

# define INT_GATE	0x8e
# define TRAP_GATE	0xef

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

extern struct idtdesc kidt[IDT_SIZE];

void _asm_default_int(void);
void _asm_page_fault(void);
void _asm_irq_clock(void);
void _asm_irq_keybord(void);

#endif
