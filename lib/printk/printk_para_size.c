/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printk_para_size.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndombre <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/09 22:37:50 by ndombre           #+#    #+#             */
/*   Updated: 2018/05/10 12:41:22 by ndombre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printk.h"

static const struct {
	char modifier[3];
	int size;
} data_ptr[] = {
	{"hh", sizeof(char)},
	{"h",  sizeof(char)},
	{"l",  sizeof(long)},
	{"ll", sizeof(long long)},
	{"j",  sizeof(intmax_t)},
	{"t",  sizeof(ptrdiff_t)},
	{"z",  sizeof(size_t)},
	{"",   0}
};

int printk_para_size(const char **fmt, struct printk_para *para)
{
	size_t i;
	size_t j;

	i = 0;
	while (data_ptr[i].modifier[0])
	{
		j = 0;
		while (1)
		{
			if (data_ptr[i].modifier[j] == '\0')
			{
				para->size = data_ptr[i].size;
				*fmt += j;
				break;
			}
			if (data_ptr[i].modifier[j] != *(*fmt + j))
				break;
			j++;
		}
		i++;
	}
	return 0;
}
