#ifndef MM_KMALLOC_H
#define MM_KMALLOC_H 1

#include <types.h>
#include <stddef.h>

struct slabent {
	struct slabent *next;
};

struct slabctl {
	/* Next free page */
	struct slabctl *next;
	struct slabctl *prev;

	/* slab entries */
	struct slabent *freelist;
};

/* Container for fixed size allocations */
struct memcache {

	size_t size;
	uint flags;
	struct slabctl *empty, *partial, *full;
};

int kmalloc_init();

#define MEMCACHE_EXT 	(1 << 0)	/* externalise slabctl from freelist */
#define MEMCACHE_FROZEN (1 << 1)	/* When allocations require a new slab, fail */

void memcache_init(struct memcache *mc, size_t size, uint flags);
void *memcache_alloc(struct memcache *mc);

int memcache_reap(struct memcache *mc);

void memcache_freeze(struct memcache *mc);
void memcache_unfreeze(struct memcache *mc);
void memcache_free(struct memcache *mc, void *);

#endif
