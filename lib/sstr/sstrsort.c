#include "lib.h"

void	sstrsort(char **list, int (*cmp)())
{
	int		i;
	char	*tmp;

	i = 0;
	while (list[i] && list[i + 1])
	{
		if ((*cmp)(list[i], list[i + 1]) > 0)
		{
			tmp = list[i];
			list[i] = list[i + 1];
			list[i + 1] = tmp;
			i = 0;
		}
		else
			i++;
	}
}
