#include <types.h>
#include <mm/mmu.h>
#include <mm/pmm.h>
#include <lib/mem.h>
#include <klog.h>

extern u32 __vmm_pud;
extern u32 __vmm_pmd;
extern u32 __vmm_pte;


#define VMM_AREA 0xffff000002000000


/* General purpose page allocator / mapping functions */
void vmm_init()
{
        klog_debug("Bootstrap vmm area\n");
        klog_debug("__vmm_pud => %h\n", &__vmm_pud);
        klog_debug("__vmm_pmd => %h\n", &__vmm_pmd);
        klog_debug("__vmm_pte => %h\n", &__vmm_pte);

        klog_debug("Zeroing vmm area...\n");
        memzero(&__vmm_pud, 4096);
        memzero(&__vmm_pmd, 4096);
        memzero(&__vmm_pte, 4096);
        klog_debug("Done zeroing...\n");
        
        klog_debug("Mapping VMM area into page structures\n")


}

void vmm_map_mmio(u64 r, u64 b, u64 e)
{
        klog_debug("Asking for a page!\n");
        
        for(int i = 0; i < 10; i++)
        {
                void *p = palloc();
                klog_debug("Got page %h\n", p);
        }



}


void *vmm_upalloc()
{

}

void *vmm_kpalloc()
{


}
