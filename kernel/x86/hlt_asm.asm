section .text
global process_hlt_user
process_hlt_user:
	hlt
	jmp process_hlt_user
	ret
