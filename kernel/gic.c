#include <types.h>
#include <gic.h>
#include <lib/common.h>
#include <lib/io.h>

#define GIC_OFFSET  0x1840000
#define GICD_OFFSET (GIC_OFFSET + 0x1000)
#define GICC_OFFSET (GIC_OFFSET + 0x2000)

static struct gic_regs gic_dev;

void gic_dump_regs()
{
        /* Getting the correct offsets is painful, so a helper to dump them and */
        /* compare to the architectural documentation */
        printk("GICC offset: %h\n", gic_dev.gicc); 
        DUMP_OFFSET(struct gicc_regs,CTLR);
        DUMP_OFFSET(struct gicc_regs,PMR);
        DUMP_OFFSET(struct gicc_regs,BPR);
        DUMP_OFFSET(struct gicc_regs,IAR);
        DUMP_OFFSET(struct gicc_regs,EOIR);
        DUMP_OFFSET(struct gicc_regs,RPR);
        DUMP_OFFSET(struct gicc_regs,HPPIR);
        DUMP_OFFSET(struct gicc_regs,ABPR);
        DUMP_OFFSET(struct gicc_regs,AIAR);
        DUMP_OFFSET(struct gicc_regs,AEOIR);
        DUMP_OFFSET(struct gicc_regs,AHPPIR);
        DUMP_OFFSET(struct gicc_regs,_reserved); 
        DUMP_OFFSET(struct gicc_regs,APR0);
        DUMP_OFFSET(struct gicc_regs,_reserved2);
        DUMP_OFFSET(struct gicc_regs,NSAPR0);
        DUMP_OFFSET(struct gicc_regs,_reserved3);
        DUMP_OFFSET(struct gicc_regs,IIDR);
        DUMP_OFFSET(struct gicc_regs,_Reserved4);
        DUMP_OFFSET(struct gicc_regs,DIR);

        printk("GICD offset: %h\n", gic_dev.gicd); 
        DUMP_OFFSET(struct gicd_regs, CTLR);
        DUMP_OFFSET(struct gicd_regs, TYPER);
        DUMP_OFFSET(struct gicd_regs, IIDR);
        DUMP_OFFSET(struct gicd_regs, _reserved0);
        DUMP_OFFSET(struct gicd_regs, IGROUP);
        DUMP_OFFSET(struct gicd_regs, _reserved1);
        DUMP_OFFSET(struct gicd_regs, ISENABLER);
        DUMP_OFFSET(struct gicd_regs, _reserved2);
        DUMP_OFFSET(struct gicd_regs, ICENABLER);
        DUMP_OFFSET(struct gicd_regs, _reserved3);
        DUMP_OFFSET(struct gicd_regs, ISPENDR);
        DUMP_OFFSET(struct gicd_regs, _reserved4);
        DUMP_OFFSET(struct gicd_regs, ICPENDR);
        DUMP_OFFSET(struct gicd_regs, _reserved5);
        DUMP_OFFSET(struct gicd_regs, ISACTIVE);
        DUMP_OFFSET(struct gicd_regs, _reserved6);
        DUMP_OFFSET(struct gicd_regs, ICACTIVE);
        DUMP_OFFSET(struct gicd_regs, _reserved7);
        DUMP_OFFSET(struct gicd_regs, IPRIORITY);
        DUMP_OFFSET(struct gicd_regs, _reserved8);
        DUMP_OFFSET(struct gicd_regs, ITARGETS);
        DUMP_OFFSET(struct gicd_regs, _reserved9);
        DUMP_OFFSET(struct gicd_regs, ICFGR);
        DUMP_OFFSET(struct gicd_regs, _reserved10);
        DUMP_OFFSET(struct gicd_regs, PPISR);
        DUMP_OFFSET(struct gicd_regs, SPISR);
        DUMP_OFFSET(struct gicd_regs, _reserved11);
        DUMP_OFFSET(struct gicd_regs, SGIR);
        DUMP_OFFSET(struct gicd_regs, _reserved12);
        DUMP_OFFSET(struct gicd_regs, CPENDSGIR);
        DUMP_OFFSET(struct gicd_regs, SPENDSGIR);
        DUMP_OFFSET(struct gicd_regs, _reserved13);
        DUMP_OFFSET(struct gicd_regs, PID);
        DUMP_OFFSET(struct gicd_regs, CID);
}

void gic_init(u64 base_addr)
{
        gic_dev.gicc = (struct gicc_regs *)(base_addr + GICC_OFFSET);
        gic_dev.gicd = (struct gicd_regs *)(base_addr + GICD_OFFSET);
}

struct gic_regs *gic_get()
{
        return &gic_dev;
}
