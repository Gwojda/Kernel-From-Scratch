#include "lib.h"

void	*memccpy(void *dst, const void *src, int c, size_t len)
{
	size_t i;

	i = 0;
	while (i < len && ((t_uchar*)src)[i] != (t_uchar)c)
	{
		((t_uchar*)dst)[i] = ((t_uchar*)src)[i];
		i++;
	}
	if (i == len)
		return (NULL);
	((t_uchar*)dst)[i] = ((t_uchar*)src)[i];
	return (dst + i + 1);
}
