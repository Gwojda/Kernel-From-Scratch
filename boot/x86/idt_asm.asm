extern irq_manager

section .text

_asm_irq_com:
	pushad
	cld
	call irq_manager
	mov al, 0x20
	out 0x20, al
	popad
	add esp, 2 * 4
	iretd

global _asm_irq_def
_asm_irq_def:
	push byte 0
	push byte -1
	jmp _asm_irq_com


%macro INT_NOERRCODE 1
	global _asm_irq_%1
	_asm_irq_%1:
		push byte 0
		push byte %1
		jmp _asm_irq_com
%endmacro

%macro INT_ERRCODE 1
	global _asm_irq_%1
	_asm_irq_%1:
		push byte %1
		jmp _asm_irq_com
%endmacro

INT_NOERRCODE 0
INT_NOERRCODE 1
INT_NOERRCODE 2
INT_NOERRCODE 3
INT_NOERRCODE 4
INT_NOERRCODE 5
INT_NOERRCODE 6
INT_NOERRCODE 7
INT_ERRCODE 8
INT_NOERRCODE 9
INT_ERRCODE 10
INT_ERRCODE 11
INT_ERRCODE 12
INT_ERRCODE 13
INT_ERRCODE 14
INT_NOERRCODE 15
INT_NOERRCODE 16
INT_NOERRCODE 17
INT_NOERRCODE 18
INT_NOERRCODE 19
INT_NOERRCODE 20
INT_NOERRCODE 21
INT_NOERRCODE 22
INT_NOERRCODE 23
INT_NOERRCODE 24
INT_NOERRCODE 25
INT_NOERRCODE 26
INT_NOERRCODE 27
INT_NOERRCODE 28
INT_NOERRCODE 29
INT_NOERRCODE 30
INT_NOERRCODE 31
INT_NOERRCODE 32
INT_NOERRCODE 33
