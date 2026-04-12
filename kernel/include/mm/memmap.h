#ifndef MM_MEMMAP_H
#define MM_MEMMAP_H 1

#include <types.h>
#include <mm/memmap.h>
#include <mm/kmalloc.h>

#define MEMMAPF_UNK         (0)	/* Unknown, memory hole / fw reserved */
#define MEMMAPF_FREE        (1)	/* Available for allocation, managed under page allocator */
#define MEMMAPF_KRES        (2)	/* Reserved as part of the kernels residency */
#define MEMMAPF_DYN			(3)	/* Memory is managed under the kernels dynamic allocator */

struct page {
	uint flags;
	uint order;
	union {
		struct page *next;
		struct slabctl *slab;
	} ctx;
};

int memmap_init();
size_t memmap_len();
struct page *memmap_entry(size_t);
struct page *memmap_vaddr(void *);
struct page *memmap_paddr(void *);
void *memmap_ptoa(struct page *);
#endif
