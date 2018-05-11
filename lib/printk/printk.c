/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printk.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndombre <ndombre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/09 15:03:10 by ndombre           #+#    #+#             */
/*   Updated: 2018/05/11 11:18:34 by gwojda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printk.h"

size_t write_str(struct printk_writer *writer, char *str)
{
	size_t i;

	i = 0;
	while (str[i])
		writer->write_char(writer, str[i++]);
	return i;
}

size_t write_char(struct printk_writer *writer, int c)
{
	(void)writer;
	vga_putchar(c);
	return 1;
}

static struct printk_writer writer = {
	.write_str = write_str,
	.write_char = write_char
};

static int printk_display(struct printk_writer *writer, const char **fmt, va_list ap)
{
	struct printk_para para = {
		.type = PRINTK_NONE,
		.size = sizeof(int),
		.is_unsigned = 0,
		.char_set = ""
	};

	if (printk_para_size(fmt, &para) != 0)
		return -1;
	if (printk_para_conv(fmt, &para) != 0)
		return -1;
	if (printk_para_display(writer, ap, &para) <= 0)
		return -1;
	return 0;
}

size_t vprintk(const char *fmt, va_list ap)
{
	size_t	ret = 0;
	int		tmp;

	while (*fmt)
	{
		if (*fmt == '%')
		{
			fmt++;
			if ((tmp = printk_display(&writer, &fmt, ap)) < 0)
			{
				writer.write_str(&writer, "BAD PRINTK FORMAT !");
				return (-1);
			}
			ret += tmp;
		}
		else
		{
			writer.write_char(&writer, *fmt);
			fmt++;
		}
	}
	return ret;
}

size_t printk(const char *fmt, ...)
{
	va_list ap;
	size_t ret;

	va_start(ap, fmt);

	ret = vprintk(fmt, ap);

	va_end(ap);
	return ret;
}
