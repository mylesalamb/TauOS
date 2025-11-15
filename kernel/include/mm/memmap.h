#ifndef MM_MEMMAP_H
#define MM_MEMMAP_H 1

#include <types.h>

#define MEMMAPF_UNK         (0)	/* Unknown, memory hole / fw reserved */
#define MEMMAPF_FREE        (1 << 0)	/* Available for allocation */
#define MEMMAPF_KRES        (1 << 1)	/* Used by the kernel */

struct page {
	uint flags;

	/* When flags = MEMMAPF_FREE */
	struct page *next;
};

int memmap_init();

#endif
