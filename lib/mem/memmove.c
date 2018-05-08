#include "lib.h"

void	*memmove(void *dst, const void *src, size_t len)
{
	if (len == 0)
		return (dst);
	if (dst <= src)
		return (memcpy(dst, src, len));
	len--;
	while (len > 0)
	{
		((char *)dst)[len] = ((char *)src)[len];
		len--;
	}
	((char *)dst)[len] = ((char *)src)[len];
	return (dst);
}
