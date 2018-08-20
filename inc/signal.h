#ifndef SIGNAL_H
# define SIGNAL_H

#include "list.h"
#include "typedef.h"

#define SIGHUP    1       /* Hangup (POSIX).  */
#define SIGINT    2       /* Interrupt (ANSI).  */
#define SIGQUIT   3       /* Quit (POSIX).  */
#define SIGILL    4       /* Illegal instruction (ANSI).  */
#define SIGTRAP   5       /* Trace trap (POSIX).  */
#define SIGABRT   6       /* Abort (ANSI).  */
#define SIGIOT    6       /* IOT trap (4.2 BSD).  */
#define SIGBUS    7       /* BUS error (4.2 BSD).  */
#define SIGFPE    8       /* Floating-point exception (ANSI).  */
#define SIGKILL   9       /* Kill, unblockable (POSIX).  */
#define SIGUSR1   10      /* User-defined signal 1 (POSIX).  */
#define SIGSEGV   11      /* Segmentation violation (ANSI).  */
#define SIGUSR2   12      /* User-defined signal 2 (POSIX).  */
#define SIGPIPE   13      /* Broken pipe (POSIX).  */
#define SIGALRM   14      /* Alarm clock (POSIX).  */
#define SIGTERM   15      /* Termination (ANSI).  */
#define SIGSTKFLT 16      /* Stack fault.  */
#define SIGCHLD   17      /* Child status has changed (POSIX).  */
#define SIGCLD    SIGCHLD /* Same as SIGCHLD (System V).  */
#define SIGCONT   18      /* Continue (POSIX).  */
#define SIGSTOP   19      /* Stop, unblockable (POSIX).  */
#define SIGTSTP   20      /* Keyboard stop (POSIX).  */
#define SIGTTIN   21      /* Background read from tty (POSIX).  */
#define SIGTTOU   22      /* Background write to tty (POSIX).  */
#define SIGURG    23      /* Urgent condition on socket (4.2 BSD).  */
#define SIGXCPU   24      /* CPU limit exceeded (4.2 BSD).  */
#define SIGXFSZ   25      /* File size limit exceeded (4.2 BSD).  */
#define SIGVTALRM 26      /* Virtual alarm clock (4.2 BSD).  */
#define SIGPROF   27      /* Profiling alarm clock (4.2 BSD).  */
#define SIGWINCH  28      /* Window size change (4.3 BSD, Sun).  */
#define SIGPOLL   SIGIO   /* Pollable event occurred (System V).  */
#define SIGIO     29      /* I/O now possible (4.2 BSD).  */
#define SIGPWR    30      /* Power failure restart (System V).  */
#define SIGSYS    31      /* Bad system call.  */

#define SIG_DFL		0	/* default signal handling */
#define SIG_IGN		1	/* ignore signal */

#define SIG_HANDLED(mask, sig)	mask |= ((u32) 1 << (sig - 1))
#define SIG_FINISHED(mask, sig)	mask &= ~((u32) 1 << (sig - 1))
#define IS_SIG(mask, sig)	(mask & ((u32) 1 << (sig - 1)))

#define SIG_AVAILABLE(mask, sig)	((mask) & ((u32) 1 << (sig - 1)))

#define DEFAULT_PROCMASK	0x00000000
#define SIGHUP_PROCMASK		(1 << SIGHUP)
#define SIGINT_PROCMASK		(1 << SIGINT)
#define SIGQUIT_PROCMASK	(1 << SIGQUIT)
#define SIGILL_PROCMASK		(1 << SIGILL)
#define SIGTRAP_PROCMASK	(1 << SIGTRAP)
#define SIGABRT_PROCMASK	(1 << SIGABRT)
#define SIGIOT_PROCMASK		(1 << SIGIOT)
#define SIGBUS_PROCMASK		(1 << SIGBUS)
#define SIGFPE_PROCMASK		(1 << SIGFPE)
#define SIGKILL_PROCMASK	0x00000000	//unblockable
#define SIGUSR1_PROCMASK	(1 << SIGUSR1)
#define SIGSEGV_PROCMASK	(1 << SIGSEGV)
#define SIGUSR2_PROCMASK	(1 << SIGUSR2)
#define SIGPIPE_PROCMASK	(1 << SIGPIPE)
#define SIGALRM_PROCMASK	(1 << SIGALRM)
#define SIGTERM_PROCMASK	(1 << SIGTERM)
#define SIGSTKFLT_PROCMASK	(1 << SIGSTKFLT)
#define SIGCHLD_PROCMASK	(1 << SIGCHLD)
#define SIGCLD_PROCMASK		(1 << SIGCLD)
#define SIGCONT_PROCMASK	(1 << SIGCONT)
#define SIGSTOP_PROCMASK	0x00000000	//unblockable
#define SIGTSTP_PROCMASK	(1 << SIGTSTP)
#define SIGTTIN_PROCMASK	(1 << SIGTTIN)
#define SIGTTOU_PROCMASK	(1 << SIGTTOU)
#define SIGURG_PROCMASK		(1 << SIGURG)
#define SIGXCPU_PROCMASK	(1 << SIGXCPU)
#define SIGXFSZ_PROCMASK	(1 << SIGXFSZ)
#define SIGVTALRM_PROCMASK	(1 << SIGVTALRM)
#define SIGPROF_PROCMASK	(1 << SIGPROF)
#define SIGWINCH_PROCMASK	(1 << SIGWINCH)
#define SIGPOLL_PROCMASK	(1 << SIGPOLL)
#define SIGIO_PROCMASK		(1 << SIGIO)
#define SIGPWR_PROCMASK		(1 << SIGPWR)
#define SIGSYS_PROCMASK		(1 << SIGSYS)

#endif
