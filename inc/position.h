#ifndef POSITION_H
# define POSITION_H

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

extern void *_kernel_rodata_start;
extern void *_kernel_rodata_end;

# define KERNEL_RODATA_START ((size_t)&_kernel_rodata_start)
# define KERNEL_RODATA_END ((size_t)&_kernel_rodata_end)

# define KERNEL_REAL_RODATA_START (KERNEL_GET_REAL(KERNEL_RODATA_START))
# define KERNEL_REAL_RODATA_END (KERNEL_GET_REAL(KERNEL_RODATA_END))

extern void *_kernel_data_start;
extern void *_kernel_data_end;

# define KERNEL_DATA_START ((size_t)&_kernel_data_start)
# define KERNEL_DATA_END ((size_t)&_kernel_data_end)

# define KERNEL_REAL_DATA_START (KERNEL_GET_REAL(KERNEL_DATA_START))
# define KERNEL_REAL_DATA_END (KERNEL_GET_REAL(KERNEL_DATA_END))

#endif
