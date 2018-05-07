#include "terminal.h"

void	terminal_putstr(const char* data)
{
	terminal_write(data, strlen(data));
}
