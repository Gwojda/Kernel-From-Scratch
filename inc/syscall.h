#ifndef SYSCALL_H
# define SYSCALL_H

#include "printk.h"
#include "process.h"

long syscall(long number, ...);
pid_t process_wait(struct process *proc, int *status);
pid_t process_waitpid(struct process *proc, pid_t waiting_on_pid, int *wstatus, int option);
pid_t fork(struct process *proc);
void exit(struct process *proc, unsigned char status);
int munmap(struct process *proc, void *addr, size_t size, int flags);
void *mmap(struct process *proc, void *addr, size_t size, int prot, int flags, int fd, size_t offset);
int getuid(struct process *proc);
int kill(struct process *proc, pid_t pid, int sig);

#endif
