#include "lib.h"

int		tolower(int c)
{
	if (c >= 65 && c <= 90)
		return (c + 32);
	return (c);
}
