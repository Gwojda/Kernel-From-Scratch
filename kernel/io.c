/*
* output byte
*/

void	outb(u32 ad, u8 v)
{
    asmv("outb %%al, %%dx" :: "d" (ad), "a" (v));;
}

/*
* output word
*/

void	outw(u32 ad, u16 v)
{
    asmv("outw %%ax, %%dx" :: "d" (ad), "a" (v));
}

/*
* output dword
*/

void	outl(u32 ad, u32 v)
{
    asmv("outl %%eax, %%dx" : : "d" (ad), "a" (v));
}

/*
* input byte
*/

u8		inb(u32 ad)
{
    u8 _v;       \
    asmv("inb %%dx, %%al" : "=a" (_v) : "d" (ad)); \
    return _v;
}

/*
* input word
*/

u16		inw(u32 ad)
{
    u16 _v;            \
    asmv("inw %%dx, %%ax" : "=a" (_v) : "d" (ad));    \
    return _v;
}

/*
* input dword
*/

u32		inl(u32 ad)
{
    u32 _v;            \
    asmv("inl %%dx, %%eax" : "=a" (_v) : "d" (ad));    \
    return _v;
}
