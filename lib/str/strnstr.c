#include "lib.h"

char	*strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (*big && strlen(little) == 0)
		return ((char *)big);
	if (len < strlen(little))
		return (NULL);
	while (big[i])
	{
		j = 0;
		while (big[i + j] && little[j] && big[i + j] == little[j]
				&& i + j < len)
			j++;
		if (little[j] == '\0' || j == len)
			return ((char *)big + i);
		i++;
	}
	return (NULL);
}
