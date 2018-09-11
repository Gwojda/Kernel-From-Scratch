#include "vga.h"
#include "lib.h"
#include "tty.h"
#include "printk.h"

void	print_initialize_status(char *init, char state)
{
	size_t i = strlen(init);
	printk("%s init . . .", init);
	for (; i < 59; ++i)
		printk(" ");
	printk("[ ");
	if (state == TRUE)
	{
		tty_setcolor(&tty[0], vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
		printk("DONE");
	}
	else
	{
		tty_setcolor(&tty[0], vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
		printk("FAIL");
	}
	tty_setcolor(&tty[0], vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	printk(" ]\n");
}
