#ifndef GIC_H
#define GIC_H 1

#include <types.h>


struct gicd_regs {
        ureg32 CTLR;
        ureg32 TYPER;
        ureg32 IIDR;
        ureg32 _reserved0[0x1E];
        ureg32 IGROUP[0x0F];
        ureg32 _reserved1[0x11];
        ureg32 ISENABLER[0x0F];
        ureg32 _reserved2[0x11];
        ureg32 ICENABLER[0x0F];
//        ureg32 _reserved3[];
//        ureg32 ISPENDR[];
//        ureg32 ICPENDR[];
//        ureg32 ISACTIVE[];
//        ureg32 ICACTIVE[];
//        ureg32 IPRIORITY[];
//        ureg32 ITARGETS[];



};

struct gicc_regs {
        ureg32 CTLR;
        ureg32 PMR;
        ureg32 BPR;
        ureg32 IAR;
        ureg32 EOIR;
        ureg32 RPR;
        ureg32 HPPIR;
        ureg32 ABPR;
        ureg32 AIAR;
        ureg32 AEOIR;
        ureg32 AHPPIR;
        ureg32 _reserved[0x2A]; // Not nice offsets
        ureg32 APR0;
        ureg32 _reserved2[0x04];
        ureg32 NSAPR0;
        ureg32 _reserved3[0x07];
        ureg32 IIDR;
        ureg32 _Reserved4[0x3c1];
        ureg32 DIR;
};

struct gic_regs {
        struct gicd_regs *gicd;
        struct gicc_regs *gicc;
};

void gic_init(u64);
struct gic_regs *gic_get();

#endif 
