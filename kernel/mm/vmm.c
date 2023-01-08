#include <mm/mmu.h>
#include <mm/pmm.h>
#include <klog.h>

/* General purpose page allocator / mapping functions */

void vmm_init()
{

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
