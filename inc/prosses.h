#ifndef PROSSES_H
# define PROSSES_H

# include "typedef.h"
# include "list.h"

typedef unsigned pid_t;
typedef unsigned uid_t;
typedef unsigned gid_t;

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
	pid_t pid;
	uid_t uid;
	struct {
		u32 eax, ecx, edx, ebx;
		u32 esp, ebp, esi, edi;
		u32 eip, eflags;
		u32 cs:16, ss:16, ds:16, es:16, fs:16, gs:16;
	} regs __attribute__ ((packed));
	struct list_head map_memory;
};

extern struct prosses *current;
extern struct list_head prosses_list;

int pros_switch(struct interupt *data, struct prosses *old, struct prosses *new);

#endif
