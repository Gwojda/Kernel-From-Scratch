/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   itoa_stack_max.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndombre <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/09 18:27:54 by ndombre           #+#    #+#             */
/*   Updated: 2018/05/10 12:34:59 by ndombre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printk.h"

static size_t itoa_stack_max_len(const char *str)
{
	size_t i;

	i = 0;
	while (*str)
	{
		str++;
		i++;
	}
	return i;
}

static size_t itoa_stack_max_rec(char *nbr_buff, unsigned long int nb, const char *char_set, size_t char_set_nb)
{
	size_t i;

	i = 0;
	if (nb >= char_set_nb)
		i = itoa_stack_max_rec(nbr_buff, nb / char_set_nb, char_set, char_set_nb);
	nbr_buff[i] = char_set[nb % char_set_nb];
	return i + 1;
}

int itoa_stack_max(char *nbr_buff, int is_unsigned, long int nb, const char *char_set)
{
	size_t last_write;
	const size_t len_charset = itoa_stack_max_len(char_set);

	if (len_charset <= 1)
		return -1;

	if (is_unsigned == 0 && nb < 0)
	{
		nbr_buff[0] = '-';

		const size_t new_nb = (-nb / len_charset);
		if (new_nb > 0)
			itoa_stack_max(nbr_buff + 1, 1, (-nb / len_charset), char_set);

		last_write = itoa_stack_max_len(nbr_buff);
		nbr_buff[last_write++] = char_set[(-(nb % (int)len_charset) + len_charset) % (int)len_charset];
		nbr_buff[last_write] = '\0';
	}
	else
	{
		last_write = itoa_stack_max_rec(nbr_buff, nb, char_set, len_charset);
		nbr_buff[last_write] = '\0';
	}
	return last_write;
}
