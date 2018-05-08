#include "lib.h"

char	*str_toupper(char *str)
{
	size_t	i;

	i = 0;
	if (!str)
		return (NULL);
	while (str[i])
	{
		str[i] = toupper(str[i]);
		++i;
	}
	return (str);
}
