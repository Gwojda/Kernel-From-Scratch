section .text
global change_root_pagin
global get_currant_page_directory

change_root_pagin:
	push ebp
	mov ebp, esp
	
	mov eax, [ebp + 8]
	mov cr3, eax

	mov esp, ebp
	pop ebp
	ret

get_currant_page_directory:
	push ebp
	mov ebp, esp

	mov eax, cr3

	mov esp, ebp
	pop ebp
	ret
