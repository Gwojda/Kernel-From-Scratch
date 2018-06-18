ENTRY(start)

SECTIONS {
	. = 0x100000;

	.boot : {
		*(.multiboot_header) /* ensure that the multiboot header is at the beginning */
		*(.boottext)
	}

	.bss : {
		*(.bss)
	}

	. += 0xC0000000;

	.text ALIGN (4K) : AT(ADDR(.text) - 0xC0000000) {
		*(.text*)
		*(.rodata*)
	}

	.data ALIGN (4K) : AT(ADDR(.data) - 0xC0000000) {
		*(.data*)
	}
}
