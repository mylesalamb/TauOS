#include <types.h>
#include <klog.h>

#define MMU_D_COUNT 512

/* Linker symbols are literally trash */
extern u32 __pgd_start;
extern u32 __pud_start;
extern u32 __pmd_start;

void mmu_dump_entries()
{
        u64 *pgd_descr = &__pgd_start; 
        u64 *pud_descr = &__pud_start;
        u64 *pmd_descr = &__pmd_start;

        klog_debug("Dump PGD entries (%h)\n", pgd_descr);
        for(u64 i = 0; i < MMU_D_COUNT; i++)
        {
                if(!pgd_descr[i])
                        continue;
                klog_debug("Entry %d: %h\n", i, pgd_descr[i]);
        }

        klog_debug("Dump PUD entries (%h)\n", pud_descr);
        for(u64 i = 0; i < MMU_D_COUNT; i++)
        {
                if(!pud_descr[i])
                        continue;
                klog_debug("Entry %d: %h\n", i, pud_descr[i]);
        }
        klog_debug("Dump PMD entries (%h)\n", pmd_descr);
        for(u64 i = 0; i < MMU_D_COUNT; i++)
        {
                if(!pmd_descr[i])
                        continue;
                klog_debug("Entry %d: %h\n", i, pmd_descr[i]);
        }
}
