#include "lib.h"

int		isprint(int c)
{
	if (isspace(c) || isalnum(c) || ispunct(c))
		return (1);
	return (0);
}
