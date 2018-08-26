#ifndef LIB_H
# define LIB_H

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

# define PI 3.141592

int		isalnum(int c);
int		isalpha(int c);
int		isascii(int c);
int		isdigit(int c);
int		islower(int c);
int		isprint(int c);
int		ispunct(int c);
int		isspace(int c);
int		isupper(int c);
int		iswhitespace(int c);
int		tolower(int c);
int		toupper(int c);

void	bzero(void *s, size_t n);
void	*memccpy(void *dst, const void *src, int c, size_t len);
void	*memchr(const void *b, int c, size_t len);
int		memcmp(const void *s1, const void *s2, size_t n);
void	*memcpy(void *dst, const void *src, size_t len);
void	*memmove(void *dst, const void *src, size_t len);
void	*memset(void *s, int c, size_t n);

void	sstrcpy(char **dst, const char **src);
size_t	sstrlen(const char **s);
void	sstrsort(char **list, int (*cmp)());
char	*sstrstr(char **sstr, char *find);

double		atof(const char *str);
int			atoi_base(const char *str, int str_base);
int			atoi(const char *nptr);
char		*str_tolower(char *str);
char		*str_toupper(char *str);
char		*strcat(char *dst, const char *src);
char		*strchr(const char *s, int c);
void		strclr(char *s);
int			strcmp(const char *s1, const char *s2);
char		*strcpy(char *dst, const char *src);
int			strequ(char const *s1, char const *s2);
void		striter(char *s, void (*f)(char *));
void		striteri(char *s, void (*f)(unsigned int, char *));
size_t		strlcat(char *dst, const char *src, size_t size);
size_t		strlen(const char* str);
char		*strncat(char *dst, const char *src, size_t n);
int			strncmp(const char *s1, const char *s2, size_t n);
char		*strncpy(char *dst, const char *src, size_t n);
int			strnequ(char const *s1, char const *s2, size_t n);
char		*strnstr(const char *big, const char *little, size_t len);
char		*strrchr(const char *s, int c);
char		*strstr(const char *big, const char *little);


#endif
