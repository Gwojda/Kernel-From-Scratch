#include "kernel.h"

static void	print_kernel_visu(void)
{
	vga_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK + current_tty));
		vga_putstr("\n"
	"                             ___---___\n"
	"                          .--         --.\n"
	"                        ./   ()      .-. \\.\n"
	"                       /   o    .   (   )  \\\n"
	"                      / .            '-'    \\\n"
	"                     | ()    .  O         .  |\n"
	"                    |                         |\n"
	"                    |    o           ()       |\n"
	"                    |       .--.          O   |\n"
	"                     | .   |    |            |\n"
	"                      \\    `.__.'    o   .  /\n"
	"                       \\                   /\n"
	"                        `\\  o    ()      /'\n"
	"                          `--___   ___--'\n"
	"                         Moonlight Kernel\n"
	"################################################################################\n"
		);
	vga_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK + current_tty));

}

void kmain(void)
{
	init_tty();
	init_vga();
	print_kernel_visu();
	init_gdt();
	launchshell();
}
