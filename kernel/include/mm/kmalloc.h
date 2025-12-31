#ifndef MM_KMALLOC_H
#define MM_KMALLOC_H 1

#include <types.h>
#include <stddef.h>
struct slabctl {
    struct slabctl *next;
    
};

/* Container for allocations */
struct memcache {

    size_t size;
    struct slabctl *partial, *full;
};

int kmalloc_init();

#endif