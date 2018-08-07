section .text
	global get_esp
	global set_esp
	global get_ebp
	global set_ebp
	global halt
	global clear_register

get_esp:
	mov eax, esp
	ret

set_esp:
	mov esp, eax
	ret

get_ebp:
	mov eax, ebp
	ret

set_ebp:
	mov ebp, eax
	ret

halt:
	cli
	hlt
	ret

clear_register:
	cli
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
;	mov cs, ax
	ltr ax
