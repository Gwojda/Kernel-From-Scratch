#include "vga.h"
#include "tty.h"
#include "printk.h"

void	print_initialize_status(char *init, char state)
{
	printk("%s init . . .                                                          [ ", init);
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
