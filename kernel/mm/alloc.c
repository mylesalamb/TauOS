#include <types.h>
#include <klog.h>
#include <stddef.h>
#include <mm/pmm.h>
#include <mm/alloc.h>

#define NULL ((void *)0)

/* hardcoded and stinky */
#define KMALLOC_MIN_SIZE 128
#define KMALLOC_MAX_SIZE 4096 
static struct kmem_cache _kmalloc_cache[6] = {};

struct kmem_page_descr *_slab_init(struct kmem_cache *c, struct kmem_page_descr *pd, u64 sz)
{
        u64 entries = sz / c->blk_size;
        klog_debug("Slab can fit %d entries\n", entries);
        klog_debug("Slab base at %h\n", pd);
        /* Partial should generally be null, but lets be safe */
        pd->next = c->partial; 
        pd->slab = (struct kmem_slab_descr *)((u8 *)pd + c->blk_size);

        /* 
         * Minus 1 for page descr at the start of the slab, minus 1 for the last
         * that we point to null
         */
        struct kmem_slab_descr *s = pd->slab;
        for(u64 i=entries - 2; i; i--)
        {
                s->block = (struct kmem_slab_descr *)((u8 *)s + c->blk_size);
                klog_debug("Wrote slab entry %h -> %h\n",s, s->block);
                s = s->block;
        }
        s->block = NULL;

        return pd;

}

void *ckmalloc(struct kmem_cache *c)
{
        if(!c->partial)
        {
                klog_debug("No more blocks in slabs, allocate a new one!\n");
                struct kmem_page_descr *pd = palloc();
                c->partial = _slab_init(c, pd, PMM_PAGE_SIZE);
        }

        struct kmem_page_descr *page = c->partial;
        void *ret = page->slab;

        // Are we out of entries yet?
        page->slab = page->slab->block;
        if(!page->slab)
        {
                // Put the next partial cache
                // as the next to alloc from
                c->partial = page->next; 

                // Push the full slab onto the full list
                page->next = c->full;
                c->full = page;

        }
        return ret;
}

void kmalloc_init()
{
        for(u64 bs = KMALLOC_MIN_SIZE, i = 0; bs <= KMALLOC_MAX_SIZE; bs <<= 1)
        {
                klog_debug("init kmalloc cache %d with size %d\n", i, bs);
                kmemcache_init(&_kmalloc_cache[i++], bs);
        }
}

void *kmalloc(u64 s)
{
        if(s > KMALLOC_MAX_SIZE)
        {
                klog_error("memory allocation that is too large! %d\n");
                return NULL;
        }

        struct kmem_cache *cache = _kmalloc_cache;

        /* Search for a cache that is big enough*/
        /* this is safe as we know that the alloc size is less than our*/
        /* largest cache */
        while(cache->blk_size < s)
                cache++;

        klog_debug("Picked cache at %h for size %d\n", cache, s);
        return ckmalloc(cache);
        
}

void kmemcache_init(struct kmem_cache *c, u64 block_size)
{
        c->flags = 0;
        c->blk_size = block_size;
        c->partial = NULL;
        c->full = NULL;
}
