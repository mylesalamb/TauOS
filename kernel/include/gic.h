#ifndef GIC_H
#define GIC_H 1

#include <types.h>


struct gicd_regs {
        ureg32 CTLR;
        ureg32 TYPER;
        ureg32 IIDR;
        ureg32 _reserved0[0x1D];
        ureg32 IGROUP[0x10];
        ureg32 _reserved1[0x10];
        ureg32 ISENABLER[0x10];
        ureg32 _reserved2[0x10];
        ureg32 ICENABLER[0x10];
        ureg32 _reserved3[0x10];
        ureg32 ISPENDR[0x10];
        ureg32 _reserved4[0x10];
        ureg32 ICPENDR[0x10];
        ureg32 _reserved5[0x10];
        ureg32 ISACTIVE[0x10];
        ureg32 _reserved6[0x10];
        ureg32 ICACTIVE[0x10];
        ureg32 _reserved7[0x10];
        ureg32 IPRIORITY[0x80];
        ureg32 _reserved8[0x80];
        ureg32 ITARGETS[0x80];
        ureg32 _reserved9[0x80];
        ureg32 ICFGR[0x20];
        ureg32 _reserved10[0x20];
        ureg32 PPISR;
        ureg32 SPISR[0xF];
        ureg32 _reserved11[0x70];
        ureg32 SGIR;
        ureg32 _reserved12[0x03];
        ureg32 CPENDSGIR[0x04];
        ureg32 SPENDSGIR[0x04];
        ureg32 _reserved13[0x28];
        ureg32 PID[8];
        ureg32 CID[4];

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
        ureg32 _reserved[0x29]; // Not nice offsets
        ureg32 APR0;
        ureg32 _reserved2[0x03];
        ureg32 NSAPR0;
        ureg32 _reserved3[0x06];
        ureg32 IIDR;
        ureg32 _reserved4[0x3c0];
        ureg32 DIR;
};

struct gic_regs {
        struct gicd_regs *gicd;
        struct gicc_regs *gicc;
};

void gic_init(u64);
struct gic_regs *gic_get();
void gic_irq_enable(u8);

extern struct irq_interface gic_interface;
#endif 
