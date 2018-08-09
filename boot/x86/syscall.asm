%define PUSH_ALL
	push ebx
	push ecx
	push edx
	push esi
	push edi
	
%define POP_ALL
	pop ebp
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx

push ebp

extern printk

extern sys_restart
	
section .data
syscall_function_array dd sys_restart,
;	dd next_syscall

arraylen equ $ - syscall_function_array

invalid_syscall_msg db "Bad syscall number", 0
caller db "addr = %p\n", 0

section .text

global syscall_handler
syscall_handler:
	cld
	cmp eax, 0
	jl invalid_syscall
	cmp eax, arraylen
	jge invalid_syscall
	PUSH_ALL
	call [syscall_function_array + eax]
	POP_ALL
	ret
;	sysret ?

invalid_syscall:
	lea eax, [invalid_syscall_msg]
	push eax
	call printk
	mov eax, -1
	ret

;use for debug
;global fake_syscall
;fake_syscall:
;	mov eax, 0
;	call syscall_handler
;	ret
