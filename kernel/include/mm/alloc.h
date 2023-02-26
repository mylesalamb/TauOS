#ifndef ALLOC_H
#define ALLOC_H 1

#include <types.h>

struct kmem_slab_descr {
        struct kmem_slab_descr *block;
};

struct kmem_page_descr {
        struct kmem_page_descr *next;
        struct kmem_slab_descr slab;
};

struct kmem_cache {
        u64 flags;

        /* Object sizes with and without metadata*/
        u64 obj_size;
        u64 blk_size;
        struct kmem_page_descr *partial, *full;

};

void *ckmalloc(struct kmem_cache *);
void *kmalloc(u64);
void kmemcache_init(struct kmem_cache *, u64);
#endif
