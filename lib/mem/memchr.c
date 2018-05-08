#include "lib.h"

void	*memchr(const void *b, int c, size_t len)
{
	size_t i;

	i = 0;
	while (i < len)
	{
		if (((t_uchar*)b)[i] == (t_uchar)c)
			return ((void*)((t_uchar*)(b + i)));
		i++;
	}
	return (NULL);
}
