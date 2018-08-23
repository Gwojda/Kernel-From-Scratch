#ifndef PROSSES_H
# define PROSSES_H

# define PROC_MEM_ADD_IMEDIATE	0b00000001
# define PROC_MEM_ADD_HEAP	0b00000010
# define PROC_MEM_ADD_STACK	0b00000100
# define PROC_MEM_ADD_CODE	0b00001000

# include "typedef.h"
# include "list.h"
# include "signal.h"
# include "idt.h"
# include "virt_mem_management.h"
# include "phys_mem_management.h"

typedef unsigned pid_t;
typedef unsigned uid_t;
typedef unsigned gid_t;

struct prosses;

typedef int (*shandler)(struct prosses *);

struct sig_queue
{
	struct list_head	list;
	u32			sig_handled;
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
	struct prosses		*p;	//reference on process, you MUSTN'T free it here
};

struct prosses
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

	struct prosses		*father;
	struct list_head	children;

	pid_t			waiting_pid;

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

extern struct prosses *current;
extern struct list_head prosses_list;

int	add_signal(int sig, struct prosses *proc);
void	send_signal(struct prosses *proc);
int pros_switch(struct interupt *data, struct prosses *old, struct prosses *new);
int prosses_memory_switch(struct prosses *pros, int add);
struct prosses	*prosses_ini_kern(u32 *v_addr, void* function, size_t size);
int		prosses_memory_add(struct prosses *pros, size_t size, void *v_addr, unsigned mflags, unsigned pflags);

#endif
