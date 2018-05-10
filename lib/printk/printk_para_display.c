/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printk_para_display.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndombre <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/09 22:39:18 by ndombre           #+#    #+#             */
/*   Updated: 2018/05/09 23:20:47 by ndombre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printk.h"

int printk_para_display(struct printk_writer *writer, va_list ap, struct printk_para *para)
{
	size_t ret = 0;

	if (para->display_prefix)
		ret += writer->write_str(writer, para->prefix);
	if (para->type == PRINTK_STRING)
	{
		if (para->size != sizeof(int))
			return -1;
		return writer->write_str(writer, va_arg(ap, char*)) + ret;
	}
	else if (para->type == PRINTK_CHAR)
	{
		if (para->size != sizeof(int))
			return -1;
		writer->write_char(writer, va_arg(ap, int));
		return 1 + ret;
	}
	else if (para->type == PRINTK_FULL_MACRO)
		return ret;
	else if (para->type == PRINTK_INT)
	{
		intmax_t nb;
		char *nbr_buff[sizeof(intmax_t) * 8 + 1] = {0};

		if (para->is_unsigned == 0)
		{
			if (para->size == sizeof(char))
				nb = va_arg(ap, int);
			else if (para->size == sizeof(short))
				nb = va_arg(ap, int);
			else if (para->size == sizeof(int))
				nb = va_arg(ap, int);
			else if (para->size == sizeof(long))
				nb = va_arg(ap, long);
			else if (para->size == sizeof(long long))
				nb = va_arg(ap, long long);
			else if (para->size == sizeof(intmax_t))
				nb = va_arg(ap, intmax_t);
			else if (para->size == sizeof(ptrdiff_t))
				nb = va_arg(ap, ptrdiff_t);
			else
				return -1;
		}
		else
		{
			if (para->size == sizeof(unsigned char))
				nb = va_arg(ap, unsigned int);
			else if (para->size == sizeof(unsigned short))
				nb = va_arg(ap, unsigned int);
			else if (para->size == sizeof(unsigned int))
				nb = va_arg(ap, unsigned int);
			else if (para->size == sizeof(unsigned long))
				nb = va_arg(ap, unsigned long);
			else if (para->size == sizeof(unsigned long long))
				nb = va_arg(ap, unsigned long long);
			else if (para->size == sizeof(uintmax_t))
				nb = va_arg(ap, uintmax_t);
			else if (para->size == sizeof(size_t))
				nb = va_arg(ap, size_t);
			else
				return -1;
		}

		itoa_stack_max((char*)&nbr_buff, para->is_unsigned, nb, para->char_set);
		return writer->write_str(writer, (char*)nbr_buff) + ret;
	}
	return 0;
}
