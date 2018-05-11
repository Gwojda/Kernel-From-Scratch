#ifndef TERMINAL_H
# define TERMINAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

volatile uint16_t*	vga_buffer;

/*
* Note the use of the volatile keyword to prevent the compiler from eliminating dead stores.
*/

void		init_vga(void);
void		vga_putchar(char c);
void		vga_putstr(const char* data);
void		vga_write(const char* data, size_t size);
void		vga_putentryat(char c, uint8_t color, size_t x, size_t y);
void		vga_setcolor(uint8_t color);
void		vga_render_tty(void);

void		vga_move_cursor(size_t x, size_t y);

uint16_t	vga_entry(unsigned char uc, uint8_t color);
uint8_t		vga_entry_color(enum vga_color fg, enum vga_color bg);

#endif
