#include "kernel.h"

void kmain(void)
{
	size_t i = 0;
	/* Initialize terminal interface */
	terminal_initialize();

	/* Newline support is left as an exercise. */
	terminal_putstr("Hello, kernel World!\n");
	while (i < 100)
	{
		terminal_putstr("blablabla\n");
		++i;
	}
}
