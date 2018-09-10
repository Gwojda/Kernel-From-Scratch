#ifndef PROCESS_H
# define PROCESS_H

# define PROC_MEM_ADD_IMEDIATE	0b00000001
# define PROC_MEM_ADD_HEAP	0b00000010
# define PROC_MEM_ADD_STACK	0b00000100
# define PROC_MEM_ADD_CODE	0b00001000

# define PROC_MIN_PID		0x1
# define PROC_MAX_PID		0xfff

# define PROT_NONE		0
# define PROT_READ		1
# define PROT_WRITE		2
# define PROT_EXEC		1

# define			SIG_DEFAULT (0b00000000)
# define			SIG_SOFT (0b00000001)
# define			SIG_HARD (0b00000010)
# define			SIG_EXECUTING (0b00000100)
# define			SIG_NOT_EXECUTING (0b00001000)

# define MAP_ANONYMOUS		1
# define MAP_ANON		MAP_ANONYMOUS
# define MAP_FIXED		2
# define MAP_USER		4
# define MAP_KERNEL_SPACE	8

# define SIG_KERNEL_SPACE	1
# define SIG_DFL		((void*)~0)
# define SIG_IGN		((void*)(~0 & ~1))

# define WAIT_EXITCODE(ret, sig)  ((ret) << 8 | (sig))
# define WAIT_STOPCODE(sig)       ((sig) << 8 | 0x7f)
# define WAIT_CONTINUED           0xffff
# define WAIT_COREFLAG             0x80

# define WNOHANG		1

# include "typedef.h"
# include "list.h"
# include "page.h"
# include "signal.h"
# include "idt.h"
# include "lib.h"
# include "virt_mem_management.h"
# include "phys_mem_management.h"

typedef int pid_t;
typedef unsigned uid_t;
typedef unsigned gid_t;

struct process;

typedef int (*shandler)(struct process *, int id);

struct sig_queue
{
	struct list_head	list;
	u32			sig_handled;
	char   			state;
};

struct signal
{
	struct sig_queue	sig_queue;
	u32			sig_avalaible;
	shandler		sig_handler[34];
};

struct map_memory
{
	struct list_head plist;
	void *v_addr;
	void *p_addr;
	size_t size;	//nb of page
	unsigned flags;
};

struct children
{
	struct list_head	list;
	struct process		*p;	//reference on process, you MUSTN'T free it here
};

struct process
{
	struct list_head plist;

	enum
	{
		RUN,
		STOPPED,
		ZOMBIE,
		THREAD
	}     state;
	pid_t pid;

	uid_t uid;

	struct process		*father;
	struct list_head	children;

	pid_t			waiting_pid;
	int			waiting_return;

	int			end_value;

	struct {
		u32 eax, ecx, edx, ebx;
		u32 esp, ebp, esi, edi;
		u32 eip, eflags;
		u32 cs:16, ss:16, ds:16, es:16, fs:16, gs:16;
	} regs __attribute__ ((packed));

	struct list_head mm_heap;
	struct list_head mm_stack;
	struct map_memory mm_code;

	struct signal signal;
};

extern struct process *current;
extern struct list_head process_list;

extern char proc_switch_context[4096];
extern struct process	*process_hlt;

int		process_memory_switch(struct process *proc, int add);
int		process_memory_add(struct process *proc, size_t size, void *v_addr, unsigned mflags, unsigned pflags);

int		add_signal(int sig, struct process *proc, int type);
void	send_signal(struct process *proc);
int		child_ended(struct process *proc, int signum);
int		process_wait(struct process *proc, pid_t waiting_on_pid, int *wstatus, int option);

void		proc_switch(struct interupt *data, struct process *old, struct process *new);
int		getuid(struct process *proc);

void	free_process(struct process *proc);
void	process_die(struct process *proc);


int				copy_process(struct process *proc, struct process *neww);
struct process	*process_new();
int		process_alloc_pid(pid_t *pid);
struct process	*process_dup(struct process *proc);
struct process	*process_ini_kern(u32 *v_addr, void* function, size_t size);
struct process *process_get_with_pid(pid_t pid);

struct process		*process_hlt_create(void);
void			process_hlt_user(void);
void switch_stack(void *, void *);
void proc_switch_iret(struct interupt);
void reload_process(void *, void *, void *);
void	process_tester(void);
void	init_process(void);

#endif
