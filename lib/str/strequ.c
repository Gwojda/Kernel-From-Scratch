#include "lib.h"

int		strequ(char const *s1, char const *s2)
{
	size_t	i;

	i = 0;
	if (!s1 || !s2)
		return (0);
	while (s1[i] == s2[i] && s1[i] && s2[i])
		i++;
	if (i == strlen(s1) && i == strlen(s2))
		return (1);
	return (0);
}
