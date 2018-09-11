#ifndef STREAM_H
# define STREAM_H

# include "typedef.h"

struct stream
{
	struct stream *next;
	size_t pos;
	size_t end;
	# define STREAM_BUFF 128
	char buff[STREAM_BUFF];
};

#include <stdatomic.h>
struct stram_containeur
{
	atomic_int	reader;
	struct stream	*next;
	struct stream	*deleted;
};

size_t stream_toread(struct stream *s);
long stream_read(struct stream *s, char *data, size_t size);
long stream_write(struct stream *s, char *data, size_t size);

#endif
