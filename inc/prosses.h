#ifndef PROSSES_H
# define PROSSES_H

# include "typedef.h"
# include "list.h"
# include "signal.h"
# include "idt.h"
# include "virt_mem_management.h"

typedef unsigned pid_t;
typedef unsigned uid_t;
typedef unsigned gid_t;

struct prosses;

typedef void (*shandler)(struct prosses *);

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
	unsigned flags;
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
	struct {
		u32 eax, ecx, edx, ebx;
		u32 esp, ebp, esi, edi;
		u32 eip, eflags;
		u32 cs:16, ss:16, ds:16, es:16, fs:16, gs:16;
	} regs __attribute__ ((packed));
	struct list_head map_memory;
	struct signal signal;
};

extern struct prosses *current;
extern struct list_head prosses_list;

int	add_signal(int sig, struct prosses *proc);
void	send_signal(struct prosses *proc);
int pros_switch(struct interupt *data, struct prosses *old, struct prosses *new);
int prosses_memory_switch(struct prosses *pros, int add);
struct prosses	*prosses_ini_kern(u32 *v_addr, void* function, size_t size);

#endif
