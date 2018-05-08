#include "lib.h"

void	striter(char *s, void (*f)(char *))
{
	if (!s || !f)
		return ;
	while (*s)
	{
		(f)(s);
		++s;
	}
}
