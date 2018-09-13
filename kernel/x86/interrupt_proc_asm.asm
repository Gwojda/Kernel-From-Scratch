global switch_stack
switch_stack:
	push ebp
	mov ebp, esp
	mov esp, [ebp + 4 * 2]
	mov eax, [ebp + 4 * 3]
	mov ebp, 0
	call eax
	ret

global proc_switch_iret
proc_switch_iret:
	add esp, 4
	pop gs
	pop fs
	pop es
	pop ds
	popad
	add esp, 8
	sti
	iretd
