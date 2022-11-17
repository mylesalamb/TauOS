#include <types.h>
#include <gic.h>
#include <irq.h>
#include <lib/common.h>
#include <lib/io.h>
#include <lib/mem.h>

#define GIC_OFFSET              0x1840000
#define GICD_OFFSET             (GIC_OFFSET + 0x1000)
#define GICC_OFFSET             (GIC_OFFSET + 0x2000)

#define GIC_TYPER_IRQ_LINES(x)  ( ((x &0x1f) + 1) * 32)
#define GIC_IAR_IRQ_NUM(x)      ( x & 0x1ff )
#define GIC_MAX_IRQ             300
#define GIC_IAR_INVALID 1023

/* Mask for core-local interrupts, these fields are preset for ITARGETS */
/* ITARGETSR_1 -> ITARGETSR_7 Should be excluded */
#define GIC_ITARGETS_LMASK (~0x7) /* 0b0111 */


static struct gic_regs gic_dev;
static void (*dispatch_table[GIC_MAX_IRQ + 1])();

void gic_init(u64);
void gic_irq_install(u32, void (*)());
void gic_handle_pending_irq();

struct irq_interface gic_interface = {
        .init = gic_init,
        .handle_pending = gic_handle_pending_irq,
        .install_handler = gic_irq_install
};

void gic_dump_regs()
{
        /* Getting the correct offsets is painful with structs, so a helper to dump them and */
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
        DUMP_OFFSET(struct gicc_regs,_reserved4);
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
        u64 irq_lines;
        gic_dev.gicc = (struct gicc_regs *)(base_addr + GICC_OFFSET);
        gic_dev.gicd = (struct gicd_regs *)(base_addr + GICD_OFFSET);
        // gic_dump_regs();

        gic_dev.gicd->CTLR = 0x00;

        irq_lines = GIC_TYPER_IRQ_LINES(gic_dev.gicd->TYPER);

        /* Deactivate all interrupts */
        for(size_t i = 0; i < irq_lines; i++){
                gic_dev.gicd->ISENABLER[i / 32] &= ~( 1 << (i % 32));
        }

        gic_dev.gicd->CTLR = 0x01;
}

void gic_irq_enable(u8 cpuid)
{
        u64 irq_lines;
        irq_lines = GIC_TYPER_IRQ_LINES(gic_dev.gicd->TYPER);
        gic_dev.gicc->CTLR &= ~(0x01);

        for(size_t i = 0; i < irq_lines; i++)
        {
                /* Register to set */
                u32 rn = i / 4;

                /* If target reg is private to the core */
                if(! (rn & GIC_ITARGETS_LMASK))
                {
                        continue;
                }

                gic_dev.gicd->ITARGETS[rn] |= 1 << (cpuid + (8 * (i % 4)));
        }

        gic_dev.gicc->PMR |= 255;
        gic_dev.gicc->CTLR |= 1;
}

void gic_irq_install(u32 irq_num, void (*f)())
{
       
        if(irq_num > GIC_MAX_IRQ)
        {
                printk("IRQ is too large for handler \n");
                return;
        }
        dispatch_table[irq_num] = f;

        gic_dev.gicd->ISENABLER[irq_num/32] |= 1 << (irq_num % 32);

}

void gic_handle_pending_irq()
{
        void (*handler)();

        u32 iar = gic_dev.gicc->IAR;
        u32 irq_number = GIC_IAR_IRQ_NUM(iar);

        printk("Interrupt id is: %h\n", iar);

        if(iar == GIC_IAR_INVALID)
        {
                printk("IAR read as invalid!\n");
                goto ack;
        }

        handler = dispatch_table[irq_number];
        if(!handler)
        {
                printk("No handler is installed for this IRQ!\n");
        }
        else
        {
                handler();
        }

ack:
        gic_dev.gicc->EOIR = iar;
}

struct gic_regs *gic_get()
{
        return &gic_dev;
}
