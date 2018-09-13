#include "process.h"

void	exit(struct process *proc, unsigned char status)
{
	asm("cli");
	proc->end_value = WAIT_EXITCODE(status, 0);
	process_die(proc);
	//while (1)
	//	asm("hlt");// TODO wait for destruction
			   // WE MUST SWITCH PROSESS ON process_die
}
