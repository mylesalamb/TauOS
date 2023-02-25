#include <types.h>
#include <mm/mm.h>
#include <mm/mmu.h>
#include <mb.h>
#include <klog.h>
#include <lib/bitmap.h>
#include <lib/mem.h>

#define GRANULE_SHIFT 12
#define GRANULE (1 << (GRANULE_SHIFT))
#define ROUNDED_DIV(n, d) ( (n / d) + ( (n % d) ? 1 : 0 ) )

struct bitmap *pages;
u64 page_count = 0;
u64 page_used = 0;
u64 page_start = 0;

void _pmm_mark(u64);
u64 _pmm_ltoi(void *);
/* Figure out where the end of the kernel is as a physical address */
void pmm_init(u64 memnd, u64 membs, void * kend)
{

        klog_debug("Allocate pmm %h - %h, from %h\n", memnd, membs, kend);
        u64 extent = memnd - membs;

        u64 entries = ROUNDED_DIV(extent, GRANULE);
        u64 page_entries = GRANULE * 8;
        u64 map_count = ROUNDED_DIV(entries, page_entries);
        klog_debug("PMM requires %d pages to map physical memory\n", map_count);

        /* where does mem start, how many pages and where are they*/
        page_start = membs;
        page_count = page_entries;
        pages = (struct bitmap *)kend;
        memset(kend, 0, map_count * GRANULE);
        
        /* Mark the pmm as used memory */
        u64 bitmap_index = _pmm_ltoi(kend);
        for(u64 i = bitmap_index; i < bitmap_index + map_count; i++ )
                _pmm_mark(i);

        klog_debug("PMM init done\n");
}

void _pmm_mark(u64 entry)
{
        bitmap_set_bit(pages, entry);
        page_used++;
}

/* logical to pmm index */
u64 _pmm_ltoi(void *p)
{
        u64 addr = (u64)p;
        addr &= ~MM_LOGICAL_START;
        addr -= page_start;
        addr /= GRANULE;
        return addr;
}

/* return the physical address of a free page */
void *palloc()
{
        for(u64 i = 0; i < page_count; i++)
        {
                if(!bitmap_is_set(pages, i))
                {
                        klog_debug("Found page idx as free\n", i);
                        bitmap_set_bit(pages, i);
                        return (void *)( MM_LOGICAL_START | ((i * GRANULE) + page_start));
                }
        }
        klog_error("PANIC: No memory left :(\n");
        while(1)
                ;
        return (void *)0;
}

void pfree(void *p)
{
        u64 addr = _pmm_ltoi(p);

        if(addr > page_count)
        {
                klog_warn("Cannot free page %h as it is not mapped\n", p);
                return;
        }

        if(!bitmap_is_set(pages, addr))
        {
                klog_warn("Double free of page %h\n", p);
                return;
        }
        bitmap_clr_bit(pages, addr);
        page_used--;
}

void pmarkrange(void * begin, void *end)
{

        u64 b = _pmm_ltoi(begin);
        u64 e = _pmm_ltoi(end);
        klog_debug("Reserving %h - %h (%d pages)\n", begin, end, (e - b));

        for(; b < e; b++)
                _pmm_mark(b);
}
