#include "lib.h"

void	strclr(char *s)
{
	if (!s)
		return ;
	bzero(s, strlen(s));
}
