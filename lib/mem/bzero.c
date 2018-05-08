#include "lib.h"

void	bzero(void *s, size_t n)
{
	while (n--)
		((t_uchar*)s)[n] = '\0';
}
