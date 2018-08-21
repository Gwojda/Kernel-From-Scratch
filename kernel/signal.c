#include "prosses.h"

static void	sig_term(struct prosses *proc)
{
	(void) proc;
	//free proc
}

static void	sig_ign(struct prosses *proc)
{
	(void) proc;
	return ;
}

static void	sig_core(struct prosses *proc)
{
	(void) proc;
	//core dump
}

static void	sig_stop(struct prosses *proc)
{
	(void) proc;
	proc->state = STOPPED;
}

static void	sig_cont(struct prosses *proc)
{
	(void) proc;
	proc->state = RUN;
}

void	send_signal(struct prosses *proc)
{
	struct sig_queue	*sig_send;
	const shandler		sig_handler[] =
	{
		sig_term,	//SIGHUP
		sig_term,	//SIGINT
		sig_core,	//SIGQUIT
		sig_core,	//SIGILL
		sig_core,	//SIGTRAP
		sig_core,	//SIGABRT
		sig_core,	//SIGIOT
		sig_core,	//SIGBUS
		sig_core,	//SIGFPE
		sig_term,	//SIGKILL
		sig_term,	//SIGUSR1
		sig_core,	//SIGSEGV
		sig_term,	//SIGUSR2
		sig_term,	//SIGPIPE
		sig_term,	//SIGALRM
		sig_term,	//SIGTERM
		sig_term,	//SIGSTKFLT
		sig_ign,	//SIGCHLD
		sig_ign,	//SIGCLD
		sig_cont,	//SIGCONT
		sig_stop,	//SIGSTOP
		sig_stop,	//SIGTSTP
		sig_stop,	//SIGTTIN
		sig_stop,	//SIGTTOU
		sig_ign,	//SIGURG
		sig_core,	//SIGXCPU
		sig_core,	//SIGXFSZ
		sig_term,	//SIGVTALRM
		sig_term,	//SIGPROF
		sig_ign,	//SIGWINCH
		sig_term,	//SIGPOLL
		sig_core,	//SIGIO
		sig_term,	//SIGPWR
		sig_core	//SIGSYS
	};

	if (proc->signal.sig_queue.list.next == &proc->signal.sig_queue.list)
		return ;
	sig_send = list_first_entry(&proc->signal.sig_queue.list, struct sig_queue, list);
	if (!proc->signal.sig_handler[sig_send->sig_handled])
		sig_handler[sig_send->sig_handled](proc);
//	else
		// en cas de syscall signal, ici c'est la partie tricky
		// repartir cote user en pushant sur la stack user de quoi revenir ici en kernel land
	list_del(&sig_send->list);
}

int	add_signal(int sig, struct prosses *proc)
{
	struct sig_queue	*new_signal;

	if (sig <= 0 || (u32)sig > (sizeof(proc->signal.sig_handler) / sizeof(shandler)))
		return -1;
	new_signal = kmalloc(sizeof(struct sig_queue));
	new_signal->sig_handled = sig;
	list_add(&new_signal->list, &proc->signal.sig_queue.list);
	return 0;
}

