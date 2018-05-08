#include "lib.h"

int		strnequ(char const *s1, char const *s2, size_t n)
{
	size_t	i;

	i = 0;
	if (!s1 || !s2)
		return (0);
	while (s1[i] == s2[i] && s1[i] && s2[i] && i < n)
		i++;
	if ((i == strlen(s1) && i == strlen(s2)) || i == n)
		return (1);
	return (0);
}
