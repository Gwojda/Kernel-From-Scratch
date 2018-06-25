/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kprint.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndombre <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/09 15:00:38 by ndombre           #+#    #+#             */
/*   Updated: 2018/05/10 13:58:03 by ndombre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRINTK_H
# define PRINTK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

size_t vprintk(const char *fmt, va_list *ap);
size_t printk(const char *fmt, ...);

void vga_putchar(char c);

enum printk_para_family
{
	PRINTK_NONE = 0,
	PRINTK_STRING,
	PRINTK_CHAR,
	PRINTK_FULL_MACRO,
	PRINTK_INT
};

struct printk_para
{
	enum printk_para_family type;
	size_t size;
	char is_unsigned;
	char *char_set;
	char *prefix;
	char display_prefix;
};

struct printk_writer
{
	size_t (*write_str)(struct printk_writer *writer, char *str);
	size_t (*write_char)(struct printk_writer *writer, int c);
};

int itoa_stack_max(char *nbr_buff, int is_unsigned, long int nb, const char *char_set);
int printk_para_conv(const char **fmt, struct printk_para *para);
int printk_para_display(struct printk_writer *writer, va_list *ap, struct printk_para *para);
int printk_para_size(const char **fmt, struct printk_para *para);

#endif
