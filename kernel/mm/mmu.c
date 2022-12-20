#include <types.h>
#include <klog.h>

#define MMU_D_COUNT 512

/* Linker symbols are literally trash */
extern u32 __pgd_start;

void mmu_dump_entries()
{
        u64 *pgd_descr = &__pgd_start; 
        for(u64 i = 0; i < MMU_D_COUNT; i++)
        {
                if(!pgd_descr[i])
                        continue;
                klog_debug("Entry %d: %h\n", i, pgd_descr[i]);
        }

}
