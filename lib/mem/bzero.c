#include "lib.h"

void	bzero(void *s, size_t n)
{
	while (n--)
		((unsigned char*)s)[n] = '\0';
}
