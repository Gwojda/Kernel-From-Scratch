#include "lib.h"

int		isalpha(int c)
{
	if (isupper(c) || islower(c))
		return (1);
	return (0);
}
