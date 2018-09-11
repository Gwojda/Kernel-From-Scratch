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

#define DEFAULT_PROCMASK	0x00000000
#define SIGHUP_PROCMASK		(1 << (SIGHUP - 1))
#define SIGINT_PROCMASK		(1 << (SIGINT - 1))
#define SIGQUIT_PROCMASK	(1 << (SIGQUIT - 1))
#define SIGILL_PROCMASK		(1 << (SIGILL - 1))
#define SIGTRAP_PROCMASK	(1 << (SIGTRAP - 1))
#define SIGABRT_PROCMASK	(1 << (SIGABRT - 1))
#define SIGIOT_PROCMASK		(1 << (SIGIOT - 1))
#define SIGBUS_PROCMASK		(1 << (SIGBUS - 1))
#define SIGFPE_PROCMASK		(1 << (SIGFPE - 1))
#define SIGKILL_PROCMASK	0x00000000	//unblockable
#define SIGUSR1_PROCMASK	(1 << (SIGUSR1 - 1))
#define SIGSEGV_PROCMASK	(1 << (SIGSEGV - 1))
#define SIGUSR2_PROCMASK	(1 << (SIGUSR2 - 1))
#define SIGPIPE_PROCMASK	(1 << (SIGPIPE - 1))
#define SIGALRM_PROCMASK	(1 << (SIGALRM - 1))
#define SIGTERM_PROCMASK	(1 << (SIGTERM - 1))
#define SIGSTKFLT_PROCMASK	(1 << (SIGSTKFLT - 1))
#define SIGCHLD_PROCMASK	(1 << (SIGCHLD - 1))
#define SIGCLD_PROCMASK		(1 << (SIGCLD - 1))
#define SIGCONT_PROCMASK	(1 << (SIGCONT - 1))
#define SIGSTOP_PROCMASK	0x00000000	//unblockable
#define SIGTSTP_PROCMASK	(1 << (SIGTSTP - 1))
#define SIGTTIN_PROCMASK	(1 << (SIGTTIN - 1))
#define SIGTTOU_PROCMASK	(1 << (SIGTTOU - 1))
#define SIGURG_PROCMASK		(1 << (SIGURG - 1))
#define SIGXCPU_PROCMASK	(1 << (SIGXCPU - 1))
#define SIGXFSZ_PROCMASK	(1 << (SIGXFSZ - 1))
#define SIGVTALRM_PROCMASK	(1 << (SIGVTALRM - 1))
#define SIGPROF_PROCMASK	(1 << (SIGPROF - 1))
#define SIGWINCH_PROCMASK	(1 << (SIGWINCH - 1))
#define SIGPOLL_PROCMASK	(1 << (SIGPOLL - 1))
#define SIGIO_PROCMASK		(1 << (SIGIO - 1))
#define SIGPWR_PROCMASK		(1 << (SIGPWR - 1))
#define SIGSYS_PROCMASK		(1 << (SIGSYS - 1))

#define SIG_UNAVAILABLE(mask, sig)	((mask) & ((u32) 1 << (sig - 1)))

#endif
