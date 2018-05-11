#include "tty.h"
#include "vga.h"

void	switch_tty(size_t new_tty)
{
	current_tty = new_tty;
	vga_render_tty();
	vga_move_cursor(tty[current_tty].tty_column, tty[current_tty].tty_row);
}
