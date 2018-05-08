#include "lib.h"

char	*strrchr(const char *s, int c)
{
	int i;

	i = strlen(s);
	while (i >= 0)
	{
		if (s[i] == (t_uchar)c)
			return ((char*)s + (t_uchar)i);
		i--;
	}
	return (NULL);
}
