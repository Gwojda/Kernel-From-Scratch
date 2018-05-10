#include "printk.h"

#include <stdio.h>

int main()
{
	printk("-5 %d\n", -5);
	printk("5 %d\n", 5);
	printk("500 %d\n", 500);
	printk("500 %u\n", 500);
	printk("500 %p\n", 500);
	printk("500 %b\n", 500);
	printk("A %c\n", 'A');
	printk("A %s\n", "A");
	printk("%%\n");
}
