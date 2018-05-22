#include "shell.h"

void reboot(void)
{
    u8 good = 0x02;

    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
    halt();
}
