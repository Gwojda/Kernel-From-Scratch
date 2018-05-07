#include "kernel.h"

void kmain(void)
{
	/* Initialize terminal interface */
	terminal_initialize();

	/* Newline support is left as an exercise. */
	terminal_putstr("Hello, kernel World!\n");
}
