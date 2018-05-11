#ifndef TTY_H
# define TTY_H

#include "vga.h"

#define MAX_TTY 10

typedef struct	s_tty
{
	size_t		tty_row;
	size_t		tty_column;
	uint8_t		tty_color;
	uint16_t	screen[VGA_WIDTH * VGA_HEIGHT];
}				t_tty;

extern size_t	current_tty;

t_tty			tty[MAX_TTY];

void	init_tty(void);
void	tty_clear_screen(size_t ind);
void	switch_tty(size_t new_tty);

#endif
