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
	cmp eax, arraylen ; ARRYLEN / 4 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

global syscall
syscall:
	push ebp
	mov ebp, esp

	PUSH_ALL
	mov eax, [ebp + 8 + 4 * 0]
	mov ebx, [ebp + 8 + 4 * 1]
	mov ecx, [ebp + 8 + 4 * 2]
	mov edx, [ebp + 8 + 4 * 3]
	mov esi, [ebp + 8 + 4 * 4]
	mov edi, [ebp + 8 + 4 * 5]
	int 0x80
	POP_ALL

	mov esp, ebp
	pop ebp
	ret

;use for debug
;global fake_syscall
;fake_syscall:
;	mov eax, 0
;	call syscall_handler
;	ret
