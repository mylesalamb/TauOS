#ifndef MM_PT_H
#define MM_PT_H 1

#define MMU_GRANULE 4096
#define MMU_BLK_LEN 512
#define MMU_BLK_SIZE (MMU_BLK_LEN * 8)

/* Hardcoded for the only granule size I support */
#define MMU_PGD_SHIFT 39
#define MMU_PUD_SHIFT 30
#define MMU_PMD_SHIFT 21
#define MMU_PTE_SHIFT 12
#define MMU_PTE_MASK 0x1ff

#define MMU_DCR_VALID           (1)
#define MMU_DCR_BLOCK           (0 << 1)
#define MMU_DCR_TABLE           (1 << 1)
#define MMU_DCR_PAGE            (1 << 1)

#define MMU_DCR_UXN             (1ULL <<  54)
#define MMU_DCR_PXN             (1ULL << 53)
#define MMU_DCR_CONTINUOUS      (1UL << 52)
#define MMU_DCR_NG              (1UL << 11)
#define MMU_DCR_AF              (1UL << 10)
#define MMU_DCR_SH_INNER        (2UL << 8)
#define MMU_DCR_SH_OUTER        (3UL << 8)
#define MMU_DCR_ATTR_AP_RW_N    (0 << 6)
#define MMU_DCR_ATTR_AP_RW_RW   (1 << 6)
#define MMU_DCR_ATTR_AP_RO_N    (2 << 6)
#define MMU_DCR_ATTR_AP_RO_RO   (3 << 6)
#define MMU_DCR_NS              (1 << 5)

#define MMU_DCR_L_ATTR_MASK     (0xfff)
#define MMU_DCR_U_ATTR_MASK     (0x3fffUL << 50)
#define MMU_DCR_ATTR_MASK       (MMU_DCR_U_ATTR_MASK | MMU_DCR_L_ATTR_MASK)

/* Derived from MAIR_EL1 see sysregs.h */
#define MMU_DCR_ATTR_NORM       (0 << 2)
#define MMU_DCR_ATTR_DEVICE     (1 << 2)

#define MMU_GRANULE_ALIGN(x)    (((x) + MMU_GRANULE - 1) & ~(MMU_GRANULE - 1))

#endif
