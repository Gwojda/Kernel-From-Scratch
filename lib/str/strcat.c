#include "lib.h"

char	*strcat(char *dst, const char *src)
{
	size_t	i;
	size_t	j;

	j = 0;
	i = 0;
	while (dst[i])
		i++;
	while (src[j])
	{
		dst[i] = src[j];
		i++;
		j++;
	}
	dst[i] = src[j];
	return (dst);
}
