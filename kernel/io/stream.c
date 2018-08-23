#include "stream.h"

size_t stream_toread(struct stream *s)
{
	if (s->pos <= s->end)
		return (s->end - s->pos);
	return (s->end + STREAM_BUFF - s->pos);
}

long stream_read(struct stream *s, char *data, size_t size)
{
	size_t readed = 0;


	while (readed < size && s->pos != s->end)
	{
		*data = s->buff[s->pos];
		data++;
		s->pos = (s->pos + 1) % STREAM_BUFF;
		readed++;
	}
	return readed;
}

long stream_write(struct stream *s, char *data, size_t size)
{
	size_t writed = 0;

	if (stream_toread(s) + size > STREAM_BUFF)
		size = STREAM_BUFF - stream_toread(s);
	while (writed < size && s->pos != (s->end + 1) % STREAM_BUFF)
	{
		s->buff[s->end] = *data++;
		s->end = (s->end + 1) % STREAM_BUFF;
		writed++;
	}
	return writed;
}
