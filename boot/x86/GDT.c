#include "GDT.h"
#include "tty.h"

struct gdtdesc 		kgdt[GDTSIZE];
struct tss 		default_tss;
struct gdtr 		kgdtr;

void init_gdt_desc(u32 base, u32 limite, u8 acces, u8 other, struct gdtdesc *desc)
{
	desc->lim0_15 = (limite & 0xffff);
	desc->base0_15 = (base & 0xffff);
	desc->base16_23 = (base & 0xff0000) >> 16;
	desc->acces = acces;
	desc->lim16_19 = (limite & 0xf0000) >> 16;
	desc->other = (other & 0xf);
	desc->base24_31 = (base & 0xff000000) >> 24;
}

void init_gdt(void)
{
	default_tss.debug_flag = 0x00;
	default_tss.io_map = 0x00;
	default_tss.esp0 = 0x1FFF0;
	default_tss.ss0 = 0x18;
	
	/* initialize gdt segments */
	init_gdt_desc(0x0, 0x0, 0x0, 0x0, &kgdt[0]);    /* default segment needed by procesor */
	
	init_gdt_desc(0x0, 0xFFFFF, 0x9B, 0x0D, &kgdt[1]);    /* code offset = 0x08 */
	init_gdt_desc(0x0, 0xFFFFF, 0x93, 0x0D, &kgdt[2]);    /* data offset = 0x10 */
	init_gdt_desc(0x0, 0x0, 0x97, 0x0D, &kgdt[3]);        /* stack offset = 0x18 */
	
	init_gdt_desc(0x0, 0xFFFFF, 0xFF, 0x0D, &kgdt[4]);    /* ucode offset = 0x20 */
	init_gdt_desc(0x0, 0xFFFFF, 0xF3, 0x0D, &kgdt[5]);    /* udata offset = 0x28 */
	init_gdt_desc(0x0, 0x0, 0xF7, 0x0D, &kgdt[6]);        /* ustack offset = 0x30 */
	
	init_gdt_desc((u32) & default_tss, 0x67, 0xE9, 0x00, &kgdt[7]);    /* descripteur de tss offset = 0x38 */
	
	/* initialize the gdtr structure */
	kgdtr.limite = GDTSIZE * 8;
	kgdtr.base = GDTBASE;
	
	/* copy the gdtr to its memory area */
	memcpy((char *) kgdtr.base, (char *) kgdt, kgdtr.limite);

	/* load the gdtr registry */
	/* Load the new GDT pointer */
	asm("lgdtl (kgdtr)");

	/* 0x38 is the offset in the GDT to our TSS */
	asm("movw $0x38, %ax");
	asm("ltr %ax");
	/* initiliaz the segments */
	asm("   movw $0x10, %ax    \n \
			movw %ax, %ds    \n \
			movw %ax, %es    \n \
			movw %ax, %fs    \n \
			movw %ax, %gs    \n \
			movw $0x18, %ax    \n \
			movw %ax, %ss    \n \
			ljmp $0x08, $next    \n \
			next:        \n");

	print_initialize_status("GDT", TRUE);
}
