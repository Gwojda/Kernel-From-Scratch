#include "process.h"
#include "errno.h"

const struct {
	u32	signal;
	u32	mask;
} blocked_sig[] = {
	{SIGHUP		, SIGHUP_PROCMASK	},
	{SIGINT		, SIGINT_PROCMASK	},
	{SIGQUIT	, SIGQUIT_PROCMASK	},
	{SIGILL		, SIGILL_PROCMASK	},
	{SIGTRAP	, SIGTRAP_PROCMASK	},
	{SIGABRT	, SIGABRT_PROCMASK	},
	{SIGIOT		, SIGIOT_PROCMASK	},
	{SIGBUS		, SIGBUS_PROCMASK	},
	{SIGFPE		, SIGFPE_PROCMASK	},
	{SIGKILL	, SIGKILL_PROCMASK	},
	{SIGUSR1	, SIGUSR1_PROCMASK	},
	{SIGSEGV	, SIGSEGV_PROCMASK	},
	{SIGUSR2	, SIGHUP_PROCMASK	},
	{SIGPIPE	, SIGPIPE_PROCMASK	},
	{SIGALRM	, SIGALRM_PROCMASK	},
	{SIGTERM	, SIGTERM_PROCMASK	},
	{SIGSTKFLT	, SIGSTKFLT_PROCMASK	},
	{SIGCHLD	, SIGCHLD_PROCMASK	},
	{SIGCLD		, SIGCLD_PROCMASK	},
	{SIGCONT	, SIGCONT_PROCMASK	},
	{SIGSTOP	, SIGSTOP_PROCMASK	},
	{SIGTSTP	, SIGTSTP_PROCMASK	},
	{SIGTTIN	, SIGTTIN_PROCMASK	},
	{SIGTTOU	, SIGTTOU_PROCMASK	},
	{SIGURG		, SIGURG_PROCMASK	},
	{SIGXCPU	, SIGXCPU_PROCMASK	},
	{SIGXFSZ	, SIGXFSZ_PROCMASK	},
	{SIGVTALRM	, SIGVTALRM_PROCMASK	},
	{SIGPROF	, SIGPROF_PROCMASK	},
	{SIGWINCH	, SIGWINCH_PROCMASK	},
	{SIGPOLL	, SIGPOLL_PROCMASK	},
	{SIGIO		, SIGIO_PROCMASK	},
	{SIGPWR		, SIGPWR_PROCMASK	},
	{SIGSYS		, SIGSYS_PROCMASK	}
}; // TODO what do you have 2 differnate tab

static int	sig_term(struct process *proc, int signum)
{
	proc->end_value = signum;
	process_die(proc);
	return 0;
}

static int	sig_ign(struct process *proc, int signum)
{
	(void)proc;
	(void)signum;
	return 0;
}

static int	sig_core(struct process *proc, int signum)
{
	proc->end_value = signum & WAIT_COREFLAG;
	//core dump
	process_die(proc);
	return 0;
}

static int	sig_stop(struct process *proc, int signum)
{
	(void)proc;
	(void)signum;
	proc->end_value = WAIT_STOPCODE(signum);
	proc->state = STOPPED;
	return 0;
}

static int	sig_cont(struct process *proc, int signum)
{
	(void)proc;
	(void)signum;
	proc->end_value = WAIT_CONTINUED;
	proc->state = RUN;
	return 0;
}

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
	child_ended,	//SIGCHLD
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

void	send_signal(struct process *proc)
{
	struct sig_queue	*sig_send;

	if (proc->signal.sig_queue.list.next == &proc->signal.sig_queue.list)
		return ;
	sig_send = list_first_entry(&proc->signal.sig_queue.list, struct sig_queue, list);
	if (sig_send->state == SIG_EXECUTING)
		return ;
	if (!proc->signal.sig_handler[sig_send->sig_handled] || sig_send->state & SIG_HARD)
		sig_handler[sig_send->sig_handled](proc, sig_send->sig_handled);
	else if ((void *)proc->signal.sig_handler[sig_send->sig_handled] > KERNEL_POS)
		proc->signal.sig_handler[sig_send->sig_handled](proc, sig_send->sig_handled);
	else
	{
		sig_send->state |= SIG_EXECUTING;	// because handler might be interrupt in user land
		// en cas de syscall signal, ici c'est la partie tricky
		// repartir cote user en pushant sur la stack user de quoi revenir ici en kernel land
	}
	if (proc->state == ZOMBIE)
		return ;
	for (size_t i = 0;i < sizeof(blocked_sig) / sizeof(*blocked_sig);++i)
	{
		if (blocked_sig[i].signal == sig_send->sig_handled)
		{
			proc->signal.sig_avalaible &= ~(blocked_sig[i].mask);
			break ;
		}
	}
	list_del(&sig_send->list);
}

int	add_signal(int sig, struct process *proc, int type)
{
	struct sig_queue	*new_signal;

	if (sig == 0)
		return 0;
	if (((type & SIG_HARD) && !(type & SIG_SOFT)) &&
	(!(type & SIG_HARD) && (type & SIG_SOFT)))
		return -EINVAL;
	if (sig < 0 || (u32)sig > (sizeof(proc->signal.sig_handler) / sizeof(shandler)))
		return -EINVAL;
	if ((type & SIG_HARD) && SIG_UNAVAILABLE(proc->signal.sig_avalaible, sig))	//signal blocked
		return 0;
	if ((new_signal = kmalloc(sizeof(struct sig_queue))) == NULL)
		return -ENOMEM;
	new_signal->sig_handled = sig;
	new_signal->state = type | SIG_NOT_EXECUTING;
	if (type & SIG_HARD)
		list_add(&new_signal->list, &proc->signal.sig_queue.list);
	else if (type & SIG_SOFT)
		list_add_end(&new_signal->list, &proc->signal.sig_queue.list);
	for (size_t i = 0;i < sizeof(blocked_sig) / sizeof(*blocked_sig);++i)
	{
		if (blocked_sig[i].signal == (u32)sig)
		{
			proc->signal.sig_avalaible |= blocked_sig[i].mask;
			break ;
		}
	}
	return 0;
}

shandler signal(struct process *proc, int signum, shandler handler/*, int flags*/)
{
	shandler old;

	if (handler == SIG_DFL)
		handler = sig_handler[signum];
	else if (handler == SIG_IGN)
		handler = sig_ign;
	else /*if (!(flags & SIG_KERNEL_SPACE))
		*/ if ((void*)handler >= KERNEL_POS)
			return (void*)-EINVAL;

	if (signum == SIGKILL || signum == SIGSTOP)
		return (void*)-EINVAL;//TODO

	old = proc->signal.sig_handler[signum];
	if (old == NULL)
		old = SIG_DFL;
	else if (old == sig_ign)
		old = SIG_IGN;
	else if ((void*)old >= KERNEL_POS)
		old = SIG_DFL; // TODO error
	proc->signal.sig_handler[signum] = handler;
	return old;
}

int kill(struct process *proc, int sig)
{
	return add_signal(sig, proc, SIG_SOFT);
}
