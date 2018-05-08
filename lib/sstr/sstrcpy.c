#include "lib.h"

void	sstrcpy(char **dst, const char **src)
{
	int y;
	int x;

	y = 0;
	while (src[y])
	{
		x = 0;
		while (src[y][x])
		{
			dst[y][x] = src[y][x];
			x++;
		}
		y++;
	}
}
