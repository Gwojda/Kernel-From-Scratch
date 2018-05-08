#include "lib.h"

void	*memccpy(void *dst, const void *src, int c, size_t len)
{
	size_t i;

	i = 0;
	while (i < len && ((unsigned char*)src)[i] != (unsigned char)c)
	{
		((unsigned char*)dst)[i] = ((unsigned char*)src)[i];
		i++;
	}
	if (i == len)
		return (NULL);
	((unsigned char*)dst)[i] = ((unsigned char*)src)[i];
	return (dst + i + 1);
}
