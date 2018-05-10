#include "terminal.h"
#include "io.h"

#define RELEASED 0
#define PRESSED 1
#define FALSE 0
#define TRUE 1
#define HALF_KEY 91

#define SHIFT_PRESSED(x) (x == 0x36 || x == 0x2A)
#define SHIFT_RELEASED(x) (x == 0xB6 || x == 0xAA)

unsigned char value_table[] = 	{
	0,
	0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
	0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
	0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0,
	'*', 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0,
	'7', '8', '9', '-',
	'4', '5', '6', '+',
	'1', '2', '3',
	'0', '.', '6',
	0, 0, 0, 0, 0,
};

unsigned char value_table_shift[] = 	{
	0,
	0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
	0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~',
	0, '|', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', 0,
	'*', 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0,
	'7', '8', '9', '-',
	'4', '5', '6', '+',
	'1', '2', '3',
	'0', '.', '6',
	0, 0, 0, 0, 0
};

void	print_keyboard_entry(void)
{
	static char shift_locked = FALSE;
	unsigned int cr;

	cr = inb(0x64);
	if (SHIFT_PRESSED(cr))
		shift_locked = TRUE;
	if (SHIFT_RELEASED(cr))
		shift_locked = FALSE;
//	terminal_putstr(itoa(cr));
//	cr &= 0x7F;
	if (shift_locked)
		terminal_putchar(value_table[cr]);
	else
		terminal_putchar(value_table_shift[cr]);
}
