#ifndef BACKTRACE_H
# define BACKTRACE_H

#include "typedef.h"

void backtrace_cur(unsigned long max);
void backtrace(size_t *ebp, unsigned long max);

#endif
