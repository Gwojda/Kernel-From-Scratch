section .text
	global get_esp
	global get_ebp
	global halt

get_esp:
	mov eax, esp
	ret

get_ebp:
	mov eax, ebp
	ret

halt:
	hlt
	ret
