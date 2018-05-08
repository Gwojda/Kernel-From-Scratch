#include "lib.h"

void	*memset(void *s, int c, size_t n)
{
	while (n--)
		((unsigned char*)s)[n] = (unsigned char)c;
	return (s);
}
