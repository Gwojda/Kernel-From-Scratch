#ifndef GDT_H
# define GDT_H

# include "lib.h"
# include "typedef.h"
# include "position.h"

# define GDTSIZE	0xFF
# define GDTBASE	KERNEL_GET_VIRTUAL(0x00000800)

# define		PAGESIZE 			4096

struct tss {
	u16 previous_task, __previous_task_unused;
	u32 esp0;
	u16 ss0, __ss0_unused;
	u32 esp1;
	u16 ss1, __ss1_unused;
	u32 esp2;
	u16 ss2, __ss2_unused;
	u32 cr3;
	u32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	u16 es, __es_unused;
	u16 cs, __cs_unused;
	u16 ss, __ss_unused;
	u16 ds, __ds_unused;
	u16 fs, __fs_unused;
	u16 gs, __gs_unused;
	u16 ldt_selector, __ldt_sel_unused;
	u16 debug_flag, io_map;
} __attribute__ ((packed));

/*
47                           16 15                         0
------------------------------------------------------------
|                             |                            |
|        BASE 16:32           |       LIMIT 0:15           |
*/

struct gdtr {
    u16 limite;
    u32 base;
} __attribute__ ((packed));

/*
63         56         51   48    45           39        32
------------------------------------------------------------
|             | |B| |A|       |P|DPL|S|0|E|W|A|            |
| BASE 31:24  |G|/|L|V| LIMIT |     LIMITS    | BASE 23:16 |

31                         16 15                         0
------------------------------------------------------------
|                             |                            |
|        BASE 15:0            |       LIMIT 15:0           |

*/

struct gdtdesc {
    u16 lim0_15;
    u16 base0_15;
    u8 base16_23;
    u8 acces;
    u8 lim16_19:4;
    u8 other:4;
    u8 base24_31;
} __attribute__ ((packed));


void init_gdt(void);

#endif
