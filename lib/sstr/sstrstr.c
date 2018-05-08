#include "lib.h"

char	*sstrstr(char **sstr, char *find)
{
	int		size;

	if (!sstr)
		return (NULL);
	size = strlen(find);
	while ((*sstr))
	{
		if (strncmp(*sstr, find, size) == 0)
			return (*sstr);
		sstr++;
	}
	return (NULL);
}
