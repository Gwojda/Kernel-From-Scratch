#ifndef POSITION_H
# define POSITION_H

# define PAGE_ALIGN(P) ((P % 4096) ? P - (P % 4096) + 4096: P)

/*
 * This file give you information about the current load of the kernel
 */

/*
 * The start of virtual memory of kernel
 */


extern void *_kernel_pos;

# define KERNEL_POS ((size_t)&_kernel_pos)
# define KERNEL_GET_REAL(P) ((size_t)(P) - KERNEL_POS)
# define KERNEL_GET_VIRTUAL(P) ((size_t)(P) + KERNEL_POS)

/*
 * The main section of kernel
 */

extern void *_kernel_start;
extern void *_kernel_end;

# define KERNEL_START ((size_t)&_kernel_start)
# define KERNEL_END ((size_t)&_kernel_end)

# define KERNEL_REAL_START (KERNEL_GET_REAL(KERNEL_START))
# define KERNEL_REAL_END (KERNEL_GET_REAL(KERNEL_END))

# define KERNEL_SIZE (KERNEL_END - KERNEL_START)

/*
 * The other section
 */

extern void *_kernel_bss_start;
extern void *_kernel_bss_end;

# define KERNEL_BSS_START ((size_t)&_kernel_bss_start)
# define KERNEL_BSS_END ((size_t)&_kernel_bss_end)

# define KERNEL_REAL_BSS_START (KERNEL_GET_REAL(KERNEL_BSS_START))
# define KERNEL_REAL_BSS_END (KERNEL_GET_REAL(KERNEL_BSS_END))

# define KERNEL_BSS_SIZE (KERNEL_BSS_END - KERNEL_BSS_START)

extern void *_kernel_text_start;
extern void *_kernel_text_end;

# define KERNEL_TEXT_START ((size_t)&_kernel_text_start)
# define KERNEL_TEXT_END ((size_t)&_kernel_text_end)

# define KERNEL_REAL_TEXT_START (KERNEL_GET_REAL(KERNEL_TEXT_START))
# define KERNEL_REAL_TEXT_END (KERNEL_GET_REAL(KERNEL_TEXT_END))

# define KERNEL_TEXT_SIZE (KERNEL_TEXT_END - KERNEL_TEXT_START)

extern void *_kernel_rodata_start;
extern void *_kernel_rodata_end;

# define KERNEL_RODATA_START ((size_t)&_kernel_rodata_start)
# define KERNEL_RODATA_END ((size_t)&_kernel_rodata_end)

# define KERNEL_REAL_RODATA_START (KERNEL_GET_REAL(KERNEL_RODATA_START))
# define KERNEL_REAL_RODATA_END (KERNEL_GET_REAL(KERNEL_RODATA_END))

# define KERNEL_RODATA_SIZE (KERNEL_RODATA_END - KERNEL_RODATA_START)

extern void *_kernel_data_start;
extern void *_kernel_data_end;

# define KERNEL_DATA_START ((size_t)&_kernel_data_start)
# define KERNEL_DATA_END ((size_t)&_kernel_data_end)

# define KERNEL_REAL_DATA_START (KERNEL_GET_REAL(KERNEL_DATA_START))
# define KERNEL_REAL_DATA_END (KERNEL_GET_REAL(KERNEL_DATA_END))

# define KERNEL_DATA_SIZE (KERNEL_DATA_END - KERNEL_DATA_START)

# define HEAP_SIZE	(0x10000)
# define HEAP_START	(0x400000 + PAGE_ALIGN(KERNEL_END))
# define HEAP_END	(HEAP_START + HEAP_SIZE)

extern void *stack_bottom;
extern void *stack_top;

# define STACK_SIZE	(0x10000)
# define STACK_START	(0x400000 + PAGE_ALIGN(HEAP_END))
# define STACK_END	(STACK_START + STACK_SIZE)


#endif
