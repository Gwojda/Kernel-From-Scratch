#include "io.h"

/*
* output byte
*/

void	outb(u32 ad, u8 v)
{
    asm("outb %%al, %%dx" :: "d" (ad), "a" (v));;
}

/*
* output word
*/

void	outw(u32 ad, u16 v)
{
    asm("outw %%ax, %%dx" :: "d" (ad), "a" (v));
}

/*
* output dword
*/

void	outl(u32 ad, u32 v)
{
    asm("outl %%eax, %%dx" : : "d" (ad), "a" (v));
}

/*
* input byte
*/

u8		inb(u32 ad)
{
    u8 _v;       \
    asm("inb %%dx, %%al" : "=a" (_v) : "d" (ad)); \
    return _v;
}

/*
* input word
*/

u16		inw(u32 ad)
{
    u16 _v;            \
    asm("inw %%dx, %%ax" : "=a" (_v) : "d" (ad));    \
    return _v;
}

/*
* input dword
*/

u32		inl(u32 ad)
{
    u32 _v;            \
    asm("inl %%dx, %%eax" : "=a" (_v) : "d" (ad));    \
    return _v;
}
