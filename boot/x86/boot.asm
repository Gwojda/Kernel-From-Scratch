%define KERNEL_POS 0xC0000000
%define PHISICAL_KERNEL_POS 0x00000000

section .bss

;   This early boot page directory
;   It will probabli be overide realy sonne
;   It's use for virtual map the kernel in hight memory

align 4096
global page_directory
page_directory:
resb 4096

section .bss

;   The multiboot standard does not define the value of the stack pointer register
;   (esp) and it is up to the kernel to provide a stack. This allocates room for a
;   small stack by creating a symbol at the bottom of it, then allocating 16384
;   bytes for it, and finally creating a symbol at the top. The stack grows
;   downwards on x86. The stack is in its own section so it can be marked nobits,
;   which means the kernel file is smaller because it does not contain an
;   uninitialized stack. The stack on x86 must be 16-byte aligned according to the
;   System V ABI standard and de-facto extensions. The compiler will assume the
;   stack is properly aligned and failure to align the stack will result in
;   undefined behavior.

global stack_top
global stack_bottom

align 4096
stack_bottom:
resb 16384 ; 16 KiB
align 4096
stack_top:

;   The linker script specifies _start as the entry point to the kernel and the
;   bootloader will jump to this position once the kernel has been loaded. It
;   doesn't make sense to return from this function as the bootloader is gone.

global _start
section .boottext
_start:
	; If you call now the real entry point, we have a problem
	; the virtual address is not the current address of the memory
	; we need to setup an early boot page management for map the memory like that:

	;	current				goal

	;	Nothing		0xC0000000	Theorique addresse of code
	;	The kernel code	0x00000000	Normaly nothing

	; So we map 0x00000000 to 0x00000000
	;       and 0xC0000000 to 0x00000000

	; we need after that to disable the link zero to hight memory kernel

	; We create empty page
	mov ecx, 1024
	mov esp, page_directory
	sub esp, KERNEL_POS
	pd_loop:
		mov DWORD [esp], 0x0
		add esp, 4
	loop pd_loop
	mov ecx, 1024

	mov esp, page_directory
	sub esp, KERNEL_POS
	; We setup the to link
	mov DWORD [esp + 4 * 0], PHISICAL_KERNEL_POS + 0x83
	mov DWORD [esp + 4 * (KERNEL_POS >> 22)], PHISICAL_KERNEL_POS + 0x83

	; We insert the page in c3
	mov ecx, page_directory
	sub ecx, KERNEL_POS
	mov cr3, ecx

	; PSE to enable 4M page
	mov esp, cr4
	or esp, 0x00000010
	mov cr4, esp
	; Let's go
	mov esp, cr0
	or esp, 0x80000001
	mov cr0, esp

	; We force a long jump
	; becose this function is in hight memory
	lea ecx, [_starthightmemory] ; load the virtual address of the real entry point
	jmp ecx

section .text
extern kmain
_starthightmemory:

;   To set up a stack, we set the esp register to point to the top of our
;   stack (as it grows downwards on x86 systems). This is necessarily done
;   in assembly as languages such as C cannot function without a stack.

	mov esp, stack_top

;   Enter the high-level kernel. The ABI requires the stack is 16-byte
;   aligned at the time of the call instruction (which afterwards pushes
;   the return pointer of size 4 bytes). The stack was originally 16-byte
;   aligned above and we've since pushed a multiple of 16 bytes to the
;   stack since (pushed 0 bytes so far) and the alignment is thus
;   preserved and the call is well defined.
	push 0
	popf
	push ebx
	push eax

	call kmain

;   If the system has nothing more to do, put the computer into an
;   infinite loop. To do that:
;   1) Disable interrupts with cli (clear interrupt enable in eflags).
;   They are already disabled by the bootloader, so this is not needed.
;   Mind that you might later enable interrupts and return from
;   kmain (which is sort of nonsensical to do).
;   2) Wait for the next interrupt to arrive with hlt (halt instruction).
;   Since they are disabled, this will lock up the computer.
;   3) Jump to the hlt instruction if it ever wakes up due to a
;   non-maskable interrupt occurring or due to system management mode.

	cli
infinite_loop:
	hlt
	jmp infinite_loop
