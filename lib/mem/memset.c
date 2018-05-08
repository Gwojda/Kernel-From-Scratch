#include "lib.h"

void	*memset(void *s, int c, size_t n)
{
	while (n--)
		((t_uchar*)s)[n] = (t_uchar)c;
	return (s);
}
