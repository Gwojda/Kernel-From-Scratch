#include "lib.h"

char	*str_tolower(char *str)
{
	size_t	i;

	i = 0;
	if (!str)
		return (NULL);
	while (str[i])
	{
		str[i] = tolower(str[i]);
		++i;
	}
	return (str);
}
