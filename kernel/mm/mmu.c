#include <types.h>
#include <mm/mmu.h>
#include <mm/pmm.h>
#include <lib/mem.h>
#include <klog.h>

#define MMU_D_COUNT 512

/* Linker symbols are literally trash */
extern u32 __pgd_start;
extern u32 __pud_start;
extern u32 __pmd_start;
extern u32 __pte_start;

/* Given a page descriptor get the address from it */
#define PG_DESCR_ADDR(x) (x & (0x7ffffffff << 12))

#define GRANULE 4096
#define ENTIRES 512

#define PGD_LEVEL 3
#define PUD_LEVEL 2
#define PMD_LEVEL 1
#define PTE_LEVEL 0

#define PTE_RANGE (ENTRIES * GRANULE)
#define PMD_RANGE (ENTRIES * PTE_RANGE)
#define PUD_RANGE (ENTRIES * PMD_RANGE)
#define PGD_RANGE (ENTRIES * PUD_RANGE)

/* Macros for extracting indexes from virtual addresses */
#define ADDR_BITS 12
#define LEVEL_SHIFT 9
#define PGD_INDEX (ADDR_BITS + (PGD_LEVEL * LEVEL_SHIFT)) 
#define PUD_INDEX (ADDR_BITS + (PUD_LEVEL * LEVEL_SHIFT))
#define PMD_INDEX (ADDR_BITS + (PMD_LEVEL * LEVEL_SHIFT))
#define PTE_INDEX (ADDR_BITS + (PTE_LEVEL * LEVEL_SHIFT))
#define PG_IDX_MASK  0x1ff
#define VADDR_IDX(x, s) ( (x >> s) & PG_IDX_MASK )

void mmu_dump_entries()
{
        u64 *pgd_descr = &__pgd_start; 
        u64 *pud_descr = &__pud_start;
        u64 *pmd_descr = &__pmd_start;
        u64 *pte_descr = &__pte_start;

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
        klog_debug("Dump PTE entries (%h)\n", pte_descr);
        for(u64 i = 0; i < MMU_D_COUNT; i++)
        {
                if(!pte_descr[i])
                        continue;
                klog_debug("Entry %d: %h\n", i, pte_descr[i]);
        }
}


void *mmu_vtp(const void *p)
{
        /* At the moment this only works for code that we have mapped in */
        /* There are real arm instructions that help with this*/

        u64 n = (u64)p;
        n &= ~0xffff000000000000;
        n += 0x80000;
        return (void *)n;
}

void *mmu_ptv(const void *p)
{
        /* Absolutely ugly hack, that will need to be removed*/
        u64 n = (u64)p;
        n -= 0x80000;
        n |= 0xffff000000000000;
        return (void *)n;
}


u64 *_mmu_deref_table(u64 *table, u64 virt_addr, u64 addr_shift)
{
        u16 pg_idx = (virt_addr >> addr_shift) & PG_IDX_MASK;
        u64 entry = table[pg_idx];
        return mmu_ptv((u64 *)PG_DESCR_ADDR(entry));

}

void _mmu_map_descr(u64 *table, u64 virt_addr, u64 addr_shift, u64 phys_addr, u64 flags)
{
        u16 pg_idx = (virt_addr >> addr_shift) & PG_IDX_MASK;
        u64 descr = phys_addr | flags;
        table[pg_idx] = descr;
        klog_debug("Mapped %h => %h\n", &table[pg_idx], table[pg_idx]);
}

/* Dumb non allocating page mapper */
/* Need this as we need to map the page allocator */
/* Bitmap before we can allocate anymore page tables */
void mmu_early_map_page(u64 virt_addr, u64 phys_addr, u64 flags)
{
        if(phys_addr & (GRANULE - 1))
        {
                klog_warn("Page is not mappable (phys) as its not page aligned\n");
        }

        if(virt_addr & (GRANULE - 1))
        {
                klog_warn("Page is not mappable (virt) as its not page aligned\n");
        }

        u64 *pgd = (u64 *)&__pgd_start;
        klog_debug("pgd addr: %h\n", pgd);

        u64 *pud = _mmu_deref_table(pgd, virt_addr, PGD_INDEX);
        klog_debug("pud addr: %h\n", pud);

        u64 *pmd = _mmu_deref_table(pud, virt_addr, PUD_INDEX);
        klog_debug("pmd addr: %h\n", pmd);

        u64 *pte = _mmu_deref_table(pmd, virt_addr, PMD_INDEX);
        klog_debug("pte addr: %h\n", pte);

        _mmu_map_descr(pte, virt_addr, PTE_INDEX, phys_addr, flags);
}
/* Given a virtual address, map the table (table), at level */
void mmu_map_table(u64 v, u64 *t, u8 l)
{

}

/* To map memory we need to be able to alter page */
void mmu_map_range(u64 virt, u64 begin, u64 end, u64 flags)
{
        if(begin & (GRANULE - 1))
        {
                klog_warn("Page is not mappable (phys) as its not page aligned\n");
        }

        if(virt & (GRANULE - 1))
        {
                klog_warn("Page is not mappable (virt) as its not page aligned\n");
        }
        u64 *pgd = (u64 *)&__pgd_start;

        u64 *pud = pgd[VADDR_IDX(virt, PGD_INDEX)];
        if(!pud)
        {
                pud = (u64 *)palloc(); 
                memzero(pud, GRANULE);
                pgd[VADDR_IDX(virt, PGD_INDEX)] = (u64)pud | MMU_TABLE_FLAGS;
        }

        u64 *pmd = pud[VADDR_IDX(virt, PUD_INDEX)];
        if(!pmd)
        {
                pmd = (u64 *)palloc(); 
                memzero(pmd, GRANULE);
                pud[VADDR_IDX(virt, PUD_INDEX)] = (u64)pmd | MMU_TABLE_FLAGS;
        }

        u64 *pte = pmd[VADDR_IDX(virt, PMD_INDEX)];
        if(!pte)
        {
                pte = (u64 *)palloc(); 
                memzero(pte, GRANULE);
                pmd[VADDR_IDX(virt, PMD_INDEX)] = (u64)pte | MMU_TABLE_FLAGS;
        }

        u64 curr = begin;
        while(curr < end)
        {
                u64 idx = pte[VADDR_IDX(virt, PTE_INDEX)];
                pte[idx] = (u64)curr | flags | MMU_DESCR_INT;

                virt += GRANULE;
                curr += GRANULE;

                if(VADDR_IDX(virt, PTE_INDEX))
                {
                        klog_error("Panic as we have overflown!\n");
                        while(1)
                                ;
                }
        }
}
