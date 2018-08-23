#include "panic.h"

void	kern_panic(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	vprintk(fmt, &ap);

	va_end(ap);
	printk("================================================================================\n");
	while (1)
		;
}
