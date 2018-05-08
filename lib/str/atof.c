#include "lib.h"

double		atof(const char *str)
{
	double	n;
	int		i;
	int		k;
	int		signe;

	i = 0;
	n = 0;
	k = 0;
	signe = 1;
	if (!str)
		return (0);
	while (str[i] == ' ' || str[i] == '\v' || str[i] == '\t' || str[i] == '\r'
			|| str[i] == '\f' || str[i] == '\n')
		i++;
	signe = (str[i] == '-') ? -signe : signe;
	i = (str[i] == '-' || str[i] == '+') ? i + 1 : i;
	while (str[i] > 47 && str[i] < 58)
	{
		n = n * 10 + (str[i++] - 48);
		if (str[i] == '.')
			k = i++;
	}
	while (k != 0 && str[++k] && str[k] > 47 && str[k] < 58)
		signe = signe * 10;
	return (n / signe);
}
