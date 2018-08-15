extern usless_function
extern irq_invalidopcode
extern irq_clock
extern irq_pagefault
extern irq_keybord
extern irq_doublefault
extern irq_divisionbyzero
extern syscall_handler
extern irq_overflow
extern irq_invalidetaskstatesegment
extern irq_segmentnotpresent
extern irq_stackfault
extern irq_generalprotection
extern irq_mathfault
extern irq_machinecheck

section .text

%macro INT_NOERRCODE 2
	global _asm_irq_%1
	_asm_irq_%1:
		push byte 0
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
		add esp, 4
		iretd
%endmacro

%macro INT_ERRCODE 2
	global _asm_irq_%1
	_asm_irq_%1:
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
		mov al, 0x20
		out 0x20, al
		popad
		add esp, 4
		iretd
%endmacro

INT_NOERRCODE 0, irq_divisionbyzero ;division by 0
INT_NOERRCODE 1, usless_function ;Single-step interrupt
INT_NOERRCODE 2, usless_function ;NMI
INT_NOERRCODE 3, usless_function ;Breakpoint (callable by the special 1-byte instruction 0xCC, used by debuggers)
INT_NOERRCODE 4, irq_overflow ;Overflow
INT_NOERRCODE 5, usless_function ;Bounds
INT_NOERRCODE 6, irq_invalidopcode ;Invalid Opcode
INT_NOERRCODE 7, usless_function ;Coprocessor not available
INT_ERRCODE 8, irq_doublefault ;Double fault
INT_NOERRCODE 9, usless_function ;Coprocessor Segment Overrun (386 or earlier only)
INT_ERRCODE 10, irq_invalidetaskstatesegment ;Invalid Task State Segment
INT_ERRCODE 11, irq_segmentnotpresent ;Segment not present
INT_ERRCODE 12, irq_stackfault ;Stack Fault
INT_ERRCODE 13, irq_generalprotection ;General protection fault
INT_ERRCODE 14, irq_pagefault ;Page fault
INT_NOERRCODE 15, usless_function ;reserved
INT_NOERRCODE 16, irq_mathfault ;Math Fault
INT_ERRCODE 17, usless_function ;Alignment Check
INT_NOERRCODE 18, irq_machinecheck ;Machine Check
INT_NOERRCODE 19, usless_function ;SIMD Floating-Point Exception
INT_NOERRCODE 20, usless_function ;Virtualization Exception
INT_NOERRCODE 21, usless_function ;Control Protection Exception
INT_NOERRCODE 22, usless_function ;reserved
INT_NOERRCODE 23, usless_function ;reserved
INT_NOERRCODE 24, usless_function ;reserved
INT_NOERRCODE 25, usless_function ;reserved
INT_NOERRCODE 26, usless_function ;reserved
INT_NOERRCODE 27, usless_function ;reserved
INT_NOERRCODE 28, usless_function ;reserved
INT_NOERRCODE 29, usless_function ;reserved
INT_ERRCODE 30, usless_function ;Security Exception
INT_NOERRCODE 31, usless_function ;reserved
INT_NOERRCODE 32, irq_clock ;Clock
INT_NOERRCODE 33, irq_keybord ;Keyboard Interrupt
INT_NOERRCODE 128, syscall_handler ;Keyboard Interrupt
