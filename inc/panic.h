#ifndef PANIC_H
# define PANIC_H

# include "printk.h"

void	kern_panic(const char *fmt, ...);

#endif
