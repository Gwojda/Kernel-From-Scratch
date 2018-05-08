#include "lib.h"

char	*strchr(const char *s, int c)
{
	size_t i;

	i = 0;
	while (i == 0 || s[i - 1])
	{
		if (s[i] == (t_uchar)c)
			return ((char*)s + (char)i);
		i++;
	}
	return (NULL);
}
