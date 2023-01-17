#ifndef VMM_H
#define VMM_H 1

#include <types.h>

/* Represents a region of memory*/
struct mm_v_area {
        u64 start;
        u64 end;
}

struct mm_p_area {
        u64 start;
        u64 end;
        u64 flags;
}


/* Allocate a contiguous region of pages */
void * vmm_alloc(u64);
void vmm_free(void *);

/* Map a region of memory, non allocating, for things like framebuffer */
struct *vmm_area vmm_map(struct *mm_v_area);
void vmm_umap(struct *vmm_area);

#endif
