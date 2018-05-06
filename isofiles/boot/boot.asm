global start
extern kmain, start_ctors, end_ctors, start_dtors, end_dtors

start:
	push ebx

static_ctors_loop:
	mov ebx, start_ctors
	jmp .test
.body:
	call [ebx]
	add ebx,4
.test:
	cmp ebx, end_ctors
	jb .body

call kmain                      ; call kernel proper

static_dtors_loop:
	mov ebx, start_dtors
	jmp .test
.body:
	call [ebx]
	add ebx,4
.test:
	cmp ebx, end_dtors
	jb .body

	cli ; stop interrupts
	hlt ; halt the CPU
