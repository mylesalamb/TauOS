#include <types.h>
#include <mm/mmu.h>
#include <mm/pmm.h>
#include <lib/mem.h>
#include <klog.h>

extern u32 __vmm_pud;
extern u32 __vmm_pmd;
extern u32 __vmm_pte;

/* Page level allocator, and concatenator, like vmalloc on linux.... if i implement it :P */
void vmm_init()
{
    klog_debug("VMM init called, but there is nothing to do :)\n");
}


void * valloc(u64 p)
{
    return (void *)0;
}