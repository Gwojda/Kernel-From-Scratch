#include "tty.h"
#include "vga.h"

void	switch_tty(struct s_tty *new_tty)
{
	current_tty = new_tty;
	tty_render_vga(current_tty);
	vga_move_cursor(current_tty->tty_column, current_tty->tty_row);
}
