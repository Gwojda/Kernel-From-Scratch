#include "lib.h"

char	*strstr(const char *big, const char *little)
{
	int i;
	int j;

	i = 0;
	j = 0;
	if (strlen(little) == 0)
		return ((char *)big);
	while (big[i])
	{
		j = 0;
		while (big[i + j] && little[j] && big[i + j] == little[j])
			j++;
		if (little[j] == '\0')
			return ((char *)big + i);
		i++;
	}
	return (NULL);
}
