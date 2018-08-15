section .ucode

global user1
user1:
	push ebp
	mov ebp, esp
	mov eax, 0
	int 0x80
	mov esp, ebp
	pop ebp
	jmp user1
	ret
