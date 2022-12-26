#include <types.h>
#include <mm/mmu.h>
#include <mb.h>
#include <klog.h>
#include <lib/bitmap.h>
#include <lib/mem.h>

extern u32 __K_END;

#define GRANULE 4096
#define VC_MEM_LENGTH(x) ( (x & 0xffffffff00000000 ) >> 32)
#define VC_MEM_BASE_ADDR(x) (x & 0xffffffff)

#define ROUNDED_DIV(n, d) ( (n / d) + ( (n % d) ? 1 : 0 ) )

struct bitmap *pages;

/* Figure out where the end of the kernel is as a physical address */
void pmm_init()
{
        void *phys_end = mmu_vtp(&__K_END);
        klog_debug("Detected end of kernel as %h (%h)\n", phys_end, &__K_END);
        /* TODO: finish bootstrapping pmm */
        u32 phys_mem = mb_get_arm_mem();
        u64 vc_mem_attrs = mb_get_vc_mem();

        klog_debug("ARM addresses exist from %h -> %h\n", 0, phys_mem);
        klog_debug("Creating PMM mapping with %d entries...\n", phys_mem / GRANULE);
        klog_debug("Reserved memory from %h, with length %h\n", VC_MEM_BASE_ADDR(vc_mem_attrs), VC_MEM_LENGTH(vc_mem_attrs));


        u64 entries = ROUNDED_DIV(phys_mem, GRANULE);
        u64 page_entries = GRANULE * 8;
        u64 map_count = ROUNDED_DIV(entries, page_entries);
        klog_debug("PMM requires %d pages to map physical memory\n", map_count);

        u64 pmm_virt = (u64)( (u8 *)&__K_END + GRANULE);
        klog_debug("Mapping from %h (%h)\n", pmm_virt, mmu_vtp(pmm_virt));


        for(int i = 0; i < map_count; i++)
        {
                klog_debug("mapping page %h (%h)\n", pmm_virt, mmu_vtp(pmm_virt));
                mmu_map_page(pmm_virt, mmu_vtp(pmm_virt), MMU_K_BLK_FLAGS | MMU_DESCR_PAGE); 
                pmm_virt += GRANULE;
        }

        pmm_virt = (u64)( (u8 *)&__K_END + GRANULE);

        klog_debug("Probe addr %h\n", (pmm_virt));
        klog_debug("%h\n", *(u64 *)(pmm_virt));
        
        memset((void *)pmm_virt, 0, map_count * GRANULE);
        klog_debug("Marked pages as free!\n");
        pages = pmm_virt;


}


void *palloc(u64 p)
{
        return (void *)0;
}

void pfree(u64 p)
{
        if(!bitmap_is_set(pages, p))
        {
                klog_warn("Double free of page %h\n", p);
        }
        bitmap_clr_bit(pages, p);
}
