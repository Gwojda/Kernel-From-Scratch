#include "lib.h"

char	*strrchr(const char *s, int c)
{
	int i;

	i = strlen(s);
	while (i >= 0)
	{
		if (s[i] == (unsigned char)c)
			return ((char*)s + (unsigned char)i);
		i--;
	}
	return (NULL);
}
