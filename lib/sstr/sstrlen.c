#include "lib.h"

size_t	sstrlen(const char **s)
{
	size_t	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}
