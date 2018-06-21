section .text
global page_directory_set
global page_directory_get

page_directory_set:
	push ebp
	mov ebp, esp
	
	mov eax, [ebp + 8]
	mov cr3, eax

	mov esp, ebp
	pop ebp
	ret

page_directory_get:
	push ebp
	mov ebp, esp

	mov eax, cr3

	mov esp, ebp
	pop ebp
	ret
