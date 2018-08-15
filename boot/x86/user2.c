__attribute__ ((section(".ucode"))) int user2(void)
{
	while (1)
		printk("hello\n");
}
