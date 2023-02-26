#include <types.h>
#include <klog.h>
#include <stddef.h>
#include <mm/pmm.h>
#include <mm/alloc.h>

#define NULL ((void *)0)

struct kmem_page_descr *_slab_init(struct kmem_cache *c, struct kmem_page_descr *pd, u64 sz)
{
        u64 entries = sz / c->blk_size;
        klog_debug("Slab can fit %d entries\n", entries);
        klog_debug("Slab base at %h\n", pd);
        /* Partial should generally be null, but lets be safe */
        pd->next = c->partial; 
        pd->slab.block = (struct kmem_slab_descr *)((u8 *)pd + c->blk_size);

        /* 
         * Minus 1 for page descr at the start of the slab, minus 1 for the last
         * that we point to null
         */

        
        struct kmem_slab_descr *s = pd->slab.block;
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
        void *ret = page->slab.block;
        page->slab.block = page->slab.block->block;
        return ret;
}

void *kmalloc(u64 s)
{
        return (void *)0;
}

void kmemcache_init(struct kmem_cache *c, u64 block_size)
{
        c->flags = 0;
        c->blk_size = block_size;
        c->partial = NULL;
        c->full = NULL;
}
