__attribute__ ((section(".ucode"))) int user2(void)
{
	int i;

	while (1)
	{
		i = 0;
		while (i++ < 10000000 / 2)
			;
		syscall(0);
	}
}

__attribute__ ((section(".ucode"))) int user3(void)
{
	int i;

	while (1)
	{
		i = 0;
		while (i++ < 10000000 / 2)
			;
		syscall(0);
	}
}

__attribute__ ((section(".ucode"))) int user_shell(void)
{
	launchshell();
}
