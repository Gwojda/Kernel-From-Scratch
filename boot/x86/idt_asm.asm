extern usless_function
extern irq_clock

section .text

%macro INT_NOERRCODE 2
	global _asm_irq_%1
	_asm_irq_%1:
		pushad
		cld
		call %2
		mov al, 0x20
		out 0x20, al
		popad
		iretd
%endmacro

%macro INT_ERRCODE 2
	global _asm_irq_%1
	_asm_irq_%1:
		pushad
		cld
		call %2
		mov al, 0x20
		out 0x20, al
		popad
		sub esp, 4
		iretd
%endmacro

INT_NOERRCODE 0, usless_function ;division by 0 
INT_NOERRCODE 1, usless_function ;Single-step interrupt
INT_NOERRCODE 2, usless_function ;NMI
INT_NOERRCODE 3, usless_function ;Breakpoint (callable by the special 1-byte instruction 0xCC, used by debuggers)
INT_NOERRCODE 4, usless_function ;Overflow
INT_NOERRCODE 5, usless_function ;Bounds
INT_NOERRCODE 6, usless_function ;Invalid Opcode
INT_NOERRCODE 7, usless_function ;Coprocessor not available
INT_ERRCODE 8, usless_function ;Double fault
INT_NOERRCODE 9, usless_function ;Coprocessor Segment Overrun (386 or earlier only)
INT_ERRCODE 10, usless_function ;Invalid Task State Segment
INT_ERRCODE 11, usless_function ;Segment not present
INT_ERRCODE 12, usless_function ;Stack Fault
INT_ERRCODE 13, usless_function ;General protection fault
INT_ERRCODE 14, usless_function ;Page fault
INT_NOERRCODE 15, usless_function ;reserved
INT_NOERRCODE 16, usless_function ;Math Fault
INT_ERRCODE 17, usless_function ;Alignment Check
INT_NOERRCODE 18, usless_function ;Machine Check
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
INT_NOERRCODE 33, usless_function ;Keyboard Interrupt
