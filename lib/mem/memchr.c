#include "lib.h"

void	*memchr(const void *b, int c, size_t len)
{
	size_t i;

	i = 0;
	while (i < len)
	{
		if (((unsigned char*)b)[i] == (unsigned char)c)
			return ((void*)((unsigned char*)(b + i)));
		i++;
	}
	return (NULL);
}
