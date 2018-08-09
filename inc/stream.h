#ifndef STREAM_H
# define STREAM_H

# include "typedef.h"

struct stream
{
	size_t pos;
	size_t end;
	# define STREAM_BUFF 8
	char buff[STREAM_BUFF];
};

size_t stream_toread(struct stream *s);
long stream_read(struct stream *s, char *data, size_t size);
long stream_write(struct stream *s, char *data, size_t size);

#endif
