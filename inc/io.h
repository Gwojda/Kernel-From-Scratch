#ifndef IO_H
# define IO_H

#include "typedef.h"

void	outb(u32 ad, u8 v);
void	outw(u32 ad, u16 v);
void	outl(u32 ad, u32 v);
u8		inb(u32 ad);
u16		inw(u32 ad);
u32		inl(u32 ad);

char	getc(void);

char	key_layout(unsigned int key);
int		set_layout(char *keymap);
char		*get_layout(void);

#endif
