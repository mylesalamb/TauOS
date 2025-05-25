#ifndef BOOT_SYSREGS_H
#define BOOT_SYSREGS_H 1
/* Intended state for system registers on boot */

/* Disable most things, and setup a 4kb translation granule */
#define TCR_EL1_T0SZ    (0x10)
#define TCR_EL1_EPD0    (0x0 << 7)
#define TCR_EL1_IRGN0   (0x0 << 8)
#define TCR_EL1_ORGN0   (0x0 << 10)
#define TCR_EL1_SH0     (0x3 << 12)
#define TCR_EL1_TG0     (0x00 << 14)
#define TCR_EL1_T1SZ    (0x10 << 16)
#define TCR_EL1_A1      (0x00 << 22)
#define TCR_EL1_EPD1    (0x00 << 23)
#define TCR_EL1_IRGN1   (0x00 << 23)
#define TCR_EL1_ORGN1   (0x00 << 26)
#define TCR_EL1_SH1     (0x00 << 28)
#define TCR_EL1_TG1     (0x02 << 30)
#define TCR_EL1_IPS     (0x05 << 32)
#define TCR_EL1_AS      (0x00 << 36)
#define TCR_EL1_TBI0    (0x0 << 37)
#define TCR_EL1_TBI1    (0x0 << 38)
#define TCR_EL1_HA      (0x0 << 39)
#define TCR_EL1_HD      (0x0 << 40)

#define TCR_EL1_VAL ( \
    TCR_EL1_T0SZ    | \
    TCR_EL1_EPD0    | \
    TCR_EL1_IRGN0   | \
    TCR_EL1_ORGN0   | \
    TCR_EL1_SH0     | \
    TCR_EL1_TG0     | \
    TCR_EL1_T1SZ    | \
    TCR_EL1_A1      | \
    TCR_EL1_EPD1    | \
    TCR_EL1_IRGN1   | \
    TCR_EL1_ORGN1   | \
    TCR_EL1_SH1     | \
    TCR_EL1_TG1     | \
    TCR_EL1_IPS     | \
    TCR_EL1_AS      | \
    TCR_EL1_TBI0    | \
    TCR_EL1_TBI1    | \
    TCR_EL1_HA      | \
    TCR_EL1_HD )

#define MAIR_EL1_0 (0x44)	/* For Normal memory, disable all inner + outer caching */
#define MAIR_EL1_1 (0x00)	/* For Device memory, disable everything */
#define MAIR_EL1_VAL    ( \
    MAIR_EL1_0          | \
    MAIR_EL1_1 << 8 )

/* Enable the mmu, but keep caches off during early boot */
#define SCTLR_EL1_M     (1)
#define SCTLR_EL1_C     (0 << 1)
#define SCTLR_EL1_SA    (1 << 3)
#define SCTLR_EL1_SA0   (1 << 4)
#define SCTLR_EL1_UMA   (1 << 9)
#define SCTLR_EL1_I     (1 << 12)

#define SCTLR_EL1_VAL  ( \
    SCTLR_EL1_M    | \
    SCTLR_EL1_C    | \
    SCTLR_EL1_SA   | \
    SCTLR_EL1_SA0  | \
    SCTLR_EL1_UMA  | \
    SCTLR_EL1_I )

#define DAIF_D (1 << 9)
#define DAIF_A (1 << 8)
#define DAIF_I (1 << 7)
#define DAIF_F (1 << 6)
#define DAIF_VAL ( \
    DAIF_D | \
    DAIF_A | \
    DAIF_I | \
    DAIF_F )
#endif
