#include "lib.h"

int		isalnum(int c)
{
	if (isdigit(c) || isalpha(c))
		return (1);
	return (0);
}
