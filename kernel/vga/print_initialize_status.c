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
		vga_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK + current_tty));
		printk("DONE");
	}
	else
	{
		vga_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK + current_tty));
		printk("FAIL");
	}
	vga_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK + current_tty));
	printk(" ]\n");
}
