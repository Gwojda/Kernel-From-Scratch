#include "lib.h"

int		isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (1);
	return (0);
}
