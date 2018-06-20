ENTRY(start)

/*
 * The kernel is in half hight memory,
 * the basse agresse is:
 */
_kernel_pos = 0xC0000000;

SECTIONS {
	. = 0x100000;

	.boot : {
		*(.multiboot_header) /* ensure that the multiboot header is at the beginning */
		*(.boottext)
	}

	. += _kernel_pos;

	_kernel_start = .;
	.text ALIGN (4K) : AT(ADDR(.text) - _kernel_pos) {
		_kernel_text_start = .;
		*(.text*)
		_kernel_text_end = .;
	}
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - _kernel_pos) {
		_kernel_rodata_start = .;
		*(.rodata*)
		_kernel_rodata_end = .;
	}
	.data ALIGN (4K) : AT(ADDR(.data) - _kernel_pos) {
		_kernel_data_start = .;
		*(.data*)
		_kernel_data_end = .;
	}

	.bss : AT(ADDR(.bss) - _kernel_pos){
		_kernel_bss_start = .;
		*(.bss)
		_kernel_bss_end = .;
	}

	_kernel_end = .;
}
