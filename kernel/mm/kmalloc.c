#include <types.h>
#include <printk.h>
#include <trace.h>
#include <mm/kmalloc.h>
#include <mm/palloc.h>

#define trace(...) _trace(CONFIG_TRACE_MM_SLAB, __VA_ARGS__)

#define KMALLOC_MIN_GRANULE 64
#define KMALLOC_MAX_ORDER 8

struct memcache _kmalloc_slabs[KMALLOC_MAX_ORDER];
struct memcache _slabctl_cache;

int kmalloc_init()
{
	trace("Test kmalloc\n");
	memcache_init(&_slabctl_cache, sizeof(struct slabctl), 0);
	for (size_t i = 0; i < KMALLOC_MAX_ORDER; i++) {
		memcache_init(&_kmalloc_slabs[i], KMALLOC_MIN_GRANULE << i, 0);
	}
	return 0;
}

void *kmalloc(size_t s)
{
	return NULL;
}

void kfree(void *p)
{

}

void memcache_init(struct memcache *mc, size_t size, uint flags)
{
	mc->flags = flags;
	mc->size = size;
	mc->partial = NULL;
	mc->empty = NULL;
	mc->full = NULL;
}

void *memcache_alloc(struct memcache *mc)
{
	struct slabctl *csr = NULL;

	if (mc->partial) {
		csr = mc->partial;
	} else if (mc->empty) {
		csr = mc->empty;
	}

	if (mc->flags & MEMCACHE_FROZEN)
		return NULL;

	return NULL;
}

void memcache_free(struct memcache *mc, void *p)
{
	(void)mc;
	(void)p;
}

void memcache_freeze(struct memcache *mc)
{
	mc->flags |= MEMCACHE_FROZEN;
}

void memcache_unfreeze(struct memcache *mc)
{
	mc->flags &= ~MEMCACHE_FROZEN;
}
