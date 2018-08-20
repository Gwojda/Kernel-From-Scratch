extern usless_function
extern irq_clock
extern irq_pagefault
extern irq_keybord
extern syscall_handler
extern irq_general

section .text

;global _asm_irq_end
;_asm_irq_end:
;		push ...
;		pop gs
;		pop fs
;		pop es
;		pop ds
;		popad
;		add esp, 4
;		iretd

%macro INT_NOERRCODE 2
	global _asm_irq_%1
	_asm_irq_%1:
		push byte 0
		push byte %1
		pushad
		push ds
		push es
		push fs
		push gs
		push ebx
		mov bx,0x10
		mov ds,bx
		pop ebx
		cld
		call %2
		pop gs
		pop fs
		pop es
		pop ds
		popad
		add esp, 8
		iretd
%endmacro

%macro INT_ERRCODE 2
	global _asm_irq_%1
	_asm_irq_%1:
		push byte %1
		pushad
		push ds
		push es
		push fs
		push gs
		push ebx
		mov bx,0x10
		mov ds,bx
		pop ebx
		cld
		call %2
		pop gs
		pop fs
		pop es
		pop ds
		popad
		add esp, 8
		iretd
%endmacro

INT_NOERRCODE 0, irq_general ;division by 0
INT_NOERRCODE 1, irq_general ;Single-step interrupt
INT_NOERRCODE 2, irq_general ;NMI
INT_NOERRCODE 3, irq_general ;Breakpoint (callable by the special 1-byte instruction 0xCC, used by debuggers)
INT_NOERRCODE 4, irq_general ;Overflow
INT_NOERRCODE 5, irq_general ;Bounds
INT_NOERRCODE 6, irq_general ;Invalid Opcode
INT_NOERRCODE 7, irq_general ;Coprocessor not available
INT_ERRCODE 8, irq_general ;Double fault
INT_NOERRCODE 9, irq_general ;Coprocessor Segment Overrun (386 or earlier only)
INT_ERRCODE 10, irq_general ;Invalid Task State Segment
INT_ERRCODE 11, irq_general ;Segment not present
INT_ERRCODE 12, irq_general ;Stack Fault
INT_ERRCODE 13, irq_general ;General protection fault
INT_ERRCODE 14, irq_pagefault ;Page fault
INT_NOERRCODE 15, irq_general ;reserved
INT_NOERRCODE 16, irq_general ;Math Fault
INT_ERRCODE 17, irq_general ;Alignment Check
INT_NOERRCODE 18, irq_general ;Machine Check
INT_NOERRCODE 19, irq_general ;SIMD Floating-Point Exception
INT_NOERRCODE 20, irq_general ;Virtualization Exception
INT_NOERRCODE 21, irq_general ;Control Protection Exception
INT_NOERRCODE 22, irq_general ;reserved
INT_NOERRCODE 23, irq_general ;reserved
INT_NOERRCODE 24, irq_general ;reserved
INT_NOERRCODE 25, irq_general ;reserved
INT_NOERRCODE 26, irq_general ;reserved
INT_NOERRCODE 27, irq_general ;reserved
INT_NOERRCODE 28, irq_general ;reserved
INT_NOERRCODE 29, irq_general ;reserved
INT_ERRCODE 30, irq_general ;Security Exception
INT_NOERRCODE 31, irq_general ;reserved
INT_NOERRCODE 32, irq_clock ;Clock
INT_NOERRCODE 33, irq_keybord ;Keyboard Interrupt
INT_NOERRCODE 128, syscall_handler ;Keyboard Interrupt
