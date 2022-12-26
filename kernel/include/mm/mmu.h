#ifndef MMU_H
#define MMU_H 1

#include <lib/common.h>

#define MMU_DESCR_ACCESS        BIT(10)
#define MMU_DESCR_VALID         BIT(0)
#define MMU_DESCR_INT           BIT(1)
#define MMU_DESCR_MAIR_SHIFT    2
#define MMU_DESCR_NS            BIT(2)
#define MMU_DESCR_EL1_RWX       BIT(54)

#define MAIR_NORM_IDX   0x0
#define MAIR_DEVICE_IDX 0x1

#define MMU_TABLE_FLAGS (MMU_DESCR_VALID | MMU_DESCR_INT )
#define MMU_K_BLK_FLAGS (MMU_DESCR_VALID | MMU_DESCR_NS | MMU_DESCR_ACCESS | MMU_DESCR_EL1_RWX | (MAIR_NORM_IDX   << 2))
#define MMU_K_DEV_FLAGS (MMU_DESCR_VALID | MMU_DESCR_NS | MMU_DESCR_ACCESS | MMU_DESCR_EL1_RWX | (MAIR_DEVICE_IDX << 2))


/* page tables that point to another level */
#define MMU_DESCR_TABLE (BIT(0) | BIT(1))
/* Block map that is not granule size but refers to a physical addr */
#define MMU_DESCR_BLOCK BIT(0)
/* Page map that refers to a physical address */
#define MMU_DESCR_PAGE (BIT(0) | BIT(1))

#define MMU_DESCR_NULL (0)


void mmu_dump_entries();
void *mmu_vtp(const void *p);
void mmu_map_page(u64, u64, u64);
#endif
