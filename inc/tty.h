#ifndef TTY_H
# define TTY_H

#include "vga.h"
#include "stream.h"

#define MAX_TTY 10

typedef struct	s_tty
{
	size_t		tty_row;
	size_t		tty_column;
	uint8_t		tty_color;
	uint16_t	screen[VGA_WIDTH * VGA_HEIGHT];
	char		input_raw[STREAM_BUFF];
	int		input_raw_c;
	struct stream	input;
}				t_tty;

extern struct s_tty	*current_tty;

t_tty			tty[MAX_TTY];

void	init_tty(void);
void	tty_scrollup(struct s_tty *term);
void	switch_tty(struct s_tty *new_tty);
void	tty_clear_screen(struct s_tty *term);

void	tty_putchar(struct s_tty *term, char c);
void	tty_putstr(struct s_tty *term, const char* data);
void	tty_write(struct s_tty *term, const char* data, size_t size);
void	tty_putentryat(struct s_tty *term, char c, uint8_t color, size_t x, size_t y);
void	tty_setcolor(struct s_tty *term, uint8_t color);
void	tty_render_vga(struct s_tty *term);

void	print_initialize_status(char *init, char state);

void	tty_input_char(struct s_tty *term, char key);

#endif
