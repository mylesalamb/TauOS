#include <types.h>
#include <klog.h>
#include <mb.h>
#include <lib/mem.h>
#include <mm/mm.h>
#include <mm/mmu.h>
#include <mm/pmm.h>

extern u32 __END;
extern u32 __START;

struct mm_area{
        u64 start;
        u64 end;
};

#define MMIO_BASE 0x0FE000000
#define MMIO_END  0x100000000

#define MM_MAP_GRANULE (1 << 21)
struct mm_area _mm_get_board_memory()
{
        /* We should get this from the device tree but for single board this is fine */
        u32 arm_mem = 0x20000000 * 2; //mb_get_arm_mem(); 
        klog_debug("Arm mem is? %h\n", arm_mem);
        struct mm_area phys = {
                .start = 0x0,
                .end = arm_mem
        };
        return phys;
}

/**
 * Initialises the memory subsystem of the operating, we begin by mapping in the remaining
 * memory of the operating system. 
 *
 * We then initialise the pmm, using the information about installed memory
 * @param dtb: The device tree to initialise drivers from
 *              as a physical address.
 */
void mm_init(struct dtb_header *dtb)
{
        klog_debug("Start mm_init\n");
        /* First we need to finish mapping the kernel into the logical space */
        struct mm_area phys = _mm_get_board_memory();

        klog_debug("discovered, board memory as %h - %h\n", phys.start, phys.end);
        for(u64 p = phys.start; p < phys.end; p += MM_MAP_GRANULE)
        {
                mmu_early_map_page(MM_LOGICAL_START | p, p, MMU_K_BLK_FLAGS);
        }

        klog_debug("Mapped in board memory, allocate pmm\n");
        pmm_init(phys.end, phys.start, &__END);
        pmarkrange(&__START, &__END);
}

/* We use the logical address space to map in devices */
/* Ideally this should be derived from the device tree */
void mm_map_peripherals()
{
        klog_info("Mapping peripherals\n");
        mmu_map_range(MM_LOGICAL_START | MMIO_BASE, MMIO_BASE, MMIO_END, MMU_K_DEV_FLAGS);

}

/* With a logical address map, we can easily map from physical to logical addresses*/
void *mm_ltp(void *p)
{
        return (void *)((u64)p & ~MM_LOGICAL_START);
}

void *mm_ptl(void *p)
{
        return (void *)((u64)p | MM_LOGICAL_START);
}


