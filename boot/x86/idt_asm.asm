extern isr_page_fault, isr_default_int, isr_clock_int, isr_keybord_int
global _asm_page_fault, _asm_default_int, _asm_irq_clock, _asm_irq_keybord

section .text
_asm_page_fault:
	add esp, 4
	pushad
	cld
	call isr_page_fault
	mov al, 0x20
	out 0x20, al
	popad
	iretd

_asm_default_int:
	pushad
	cld
	call isr_default_int
	mov al, 0x20
	out 0x20, al
	popad
	iretd

_asm_irq_clock:
	pushad
	cld
	call isr_clock_int
	mov al, 0x20
	out 0x20, al
	popad
	iretd

_asm_irq_keybord:
	pushad
	cld
	mov al, 0x20
	out 0x20, al
	call isr_keybord_int
	popad
	iretd
