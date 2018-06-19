ENTRY(start)

/*
 * The kernel is in half hight memory,
 * the basse agresse is:
 */
KERNEL_POS = 0xC0000000;

SECTIONS {
	. = 0x100000;

	.boot : {
		*(.multiboot_header) /* ensure that the multiboot header is at the beginning */
		*(.boottext)
	}

	.bss : {
		*(.bss)
	}

	. += KERNEL_POS;

	.text ALIGN (4K) : AT(ADDR(.text) - KERNEL_POS) {
		*(.text*)
		*(.rodata*)
	}

	.data ALIGN (4K) : AT(ADDR(.data) - KERNEL_POS) {
		*(.data*)
	}
}
