#include "lib.h"

int		atoi(const char *nptr)
{
	size_t	i;
	int		result;
	char	sign;

	i = 0;
	result = 0;
	sign = 1;
	while (nptr[i] == ' ' || nptr[i] == '\t' || nptr[i] == '\n'
	|| nptr[i] == '\v' || nptr[i] == '\r' || nptr[i] == '\f')
		i++;
	if (nptr[i] == '-')
		sign = -1;
	if (nptr[i] == '+' || nptr[i] == '-')
		i++;
	while (nptr[i] && (nptr[i] >= 48 && nptr[i] <= 57))
	{
		result = result * 10 + (nptr[i] - '0');
		i++;
	}
	return (sign * result);
}
