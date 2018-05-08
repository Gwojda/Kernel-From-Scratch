#include "lib.h"

size_t	strlcat(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	dst_len;

	i = 0;
	dst_len = strlen(dst);
	if (size <= dst_len)
		return (size + strlen(src));
	while (src[i] && i < (size - dst_len - 1))
	{
		dst[dst_len + i] = src[i];
		i++;
	}
	dst[dst_len + i] = '\0';
	return (dst_len + strlen(src));
}
