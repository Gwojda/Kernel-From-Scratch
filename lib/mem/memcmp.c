#include "lib.h"

int		memcmp(const void *s1, const void *s2, size_t n)
{
	size_t	i;
	int		diff;

	i = 0;
	diff = 0;
	while (i < n)
	{
		diff = ((unsigned char*)s1)[i] - ((unsigned char*)s2)[i];
		if (diff != 0)
			return (diff);
		i++;
	}
	return (0);
}
