#ifndef AUX_H
#define AUX_H 1

#include <types.h>

#define PHY_OFFSET_AUX_REGS (u64)0x215000

struct aux_regs {
        ureg32 AUX_IRQ;
        ureg32 AUX_ENABLES;
        ureg32 reserved[14];
        ureg32 AUX_MU_IO_REG;
        ureg32 AUX_MU_IER_REG;
        ureg32 AUX_MU_IIR_REG;
        ureg32 AUX_MU_LCR_REG;
        ureg32 AUX_MU_MCR_REG;
        ureg32 AUX_MU_LSR_REG;
        ureg32 AUX_MU_MSR_REG;
        ureg32 AUX_MU_SCRATCH;
        ureg32 AUX_MU_CNTL_REG;
        ureg32 AUX_MU_STAT_REG;
        ureg32 AUX_MU_BAUD_REG;
};

struct aux_regs *aux_init(u64);
struct aux_regs *aux_get();

#endif
