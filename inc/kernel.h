#ifndef KERNEL_H
# define KERNEL_H

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

# include "vga.h"
# include "lib.h"
# include "tty.h"
# include "printk.h"
# include "io.h"

# if !defined(__i386__)
#  error "This tutorial needs to be compiled with a ix86-elf compiler"
# endif

size_t	strlen(const char* str);

#endif
