#include "lib.h"

int		memcmp(const void *s1, const void *s2, size_t n)
{
	size_t	i;
	int		diff;

	i = 0;
	diff = 0;
	while (i < n)
	{
		diff = ((t_uchar*)s1)[i] - ((t_uchar*)s2)[i];
		if (diff != 0)
			return (diff);
		i++;
	}
	return (0);
}
