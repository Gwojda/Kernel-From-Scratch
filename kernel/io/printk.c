#include "printk.h"
#include "tty.h"

struct printk_writer_tty
{
	struct printk_writer pw;
	struct s_tty *term;
};

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
	struct printk_writer_tty *wt = (struct printk_writer_tty*)writer;
	tty_putchar(wt->term, c);
	return 1;
}

size_t vprintk(const char *fmt, va_list *ap)
{
	struct printk_writer_tty writer = {
		.pw = {
			.write_str = write_str,
			.write_char = write_char
		}
	};
	writer.term = current_tty;

	return wvprintk((struct printk_writer*)&writer, fmt, ap);
}

size_t printk(const char *fmt, ...)
{
	va_list ap;
	size_t ret;

	va_start(ap, fmt);

	ret = vprintk(fmt, &ap);

	va_end(ap);
	return ret;
}
