#ifndef PANIC_H
# define PANIC_H

# include "printk.h"

void	kern_panic(const char *fmt, ...);
void	clear_register(void);

#endif
