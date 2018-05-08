#include "lib.h"

int		toupper(int c)
{
	if (c >= 97 && c <= 122)
		return (c - 32);
	return (c);
}
