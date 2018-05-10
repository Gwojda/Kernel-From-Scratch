/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printk_para_conv.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndombre <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/09 22:38:38 by ndombre           #+#    #+#             */
/*   Updated: 2018/05/10 12:37:58 by ndombre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printk.h"

static const struct {
	char convertion;
	enum printk_para_family type;
	int is_unsigned;
	int size;
	char char_set[17];
	char prefix[3];
	char display_prefix;
} data_ptr[] = {
	{'d', PRINTK_INT,        0, 0,            "0123456789",      "",   0},
	{'b', PRINTK_INT,        1, 0,            "01",              "0b", 1},
	{'i', PRINTK_INT,        0, 0,            "0123456789",      "",   0},
	{'o', PRINTK_INT,        1, 0,            "01234567",        "0",  0},
	{'u', PRINTK_INT,        1, 0,            "0123456789",      "",   0},
	{'x', PRINTK_INT,        1, 0,            "0123456789abcde", "0x", 0},
	{'p', PRINTK_INT,        1, 0,            "0123456789abcde", "0x", 1},
	{'X', PRINTK_INT,        1, 0,            "0123456789ABCDE", "0X", 0},
	{'D', PRINTK_INT,        1, sizeof(long), "0123456789",      "",   0},
	{'O', PRINTK_INT,        1, sizeof(long), "01234567",        "",   0},

	{'c', PRINTK_CHAR,       0, 0,            "",                "",   0},
	{'C', PRINTK_CHAR,       0, sizeof(long), "",                "",   0},

	{'%', PRINTK_FULL_MACRO, 0, 0,            "",                "%",  1},

	{'s', PRINTK_STRING,     0, 0,            "",                "",   0},
	{'S', PRINTK_STRING,     0, sizeof(long), "",                "",   0},

	{'\0', PRINTK_NONE,      0, 0,            "",                "",   0}
};

int printk_para_conv(const char **fmt, struct printk_para *para)
{
	size_t i;

	i = 0;
	while (data_ptr[i].convertion)
	{
		if (data_ptr[i].convertion == **fmt)
		{
			para->type = data_ptr[i].type;
			para->is_unsigned = data_ptr[i].is_unsigned;
			if (data_ptr[i].size)
			{
				/*
				 * Error : the size is already set
				 */
				if (para->size != sizeof(int))
					return (-1);
				para->size = data_ptr[i].size;
			}
			para->char_set = (char*)&(data_ptr[i].char_set);
			para->prefix = (char*)&(data_ptr[i].prefix);
			if (data_ptr[i].display_prefix)
				para->display_prefix = data_ptr[i].display_prefix;
			(*fmt)++;
			break;
		}
		i++;
	}
	return data_ptr[i].convertion == 0;
}
