#include "kernel.h"

void kmain(void)
{
	vga_putstr("\n"
"         ___---___\n"
"      .--         --.\n"
"    ./   ()      .-. \\.\n"
"   /   o    .   (   )  \\\n"
"  / .            '-'    \\\n"
" | ()    .  O         .  |\n"
"|                         |\n"
"|    o           ()       |\n"
"|       .--.          O   |\n"
" | .   |    |            |\n"
"  \\    `.__.'    o   .  /\n"
"   \\                   /\n"
"    `\\  o    ()      /'\n"
"      `--___   ___--'\n"
"            ---\n");
	printk("Prink test ! %d \n", 4242);
	while (1)
		vga_putchar(getc());
}
