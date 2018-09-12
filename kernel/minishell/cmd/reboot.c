#include "shell.h"
#include "idt.h"

struct idtr fakekidtr;

void reboot(void)
{
	fakekidtr.limite = 0;
	asm("lidt (fakekidtr)");

	*((char *)0) = 42;
/*
 *	used for x86 proc
	u8 good = 0x02;

	while (good & 0x02)
		good = inb(0x64);
	outb(0x64, 0xFE);
	halt();
*/
}
