#include <types.h>
#include <irq.h>
#include <lib/io.h>
#include <lib/common.h>
#include <gic.h>

struct irq_interface *irq_dev;

void irq_init(struct irq_interface *dev, u64 base_addr)
{
        irq_dev = dev;
        irq_dev->init(base_addr);
        irq_init_vectors();

}

/* Dont think we should really expose the actual device */
void irq_install_handler(u32 irq_number, void (*handle)())
{
        irq_dev->install_handler(irq_number, handle);
}

void se_rcver(struct irq_ctx *ctx)
{
        printk("Synchronous Exception occured!\n");
        for(int i = 0; i < ARRAY_SZ(ctx->registers); i++)
        {
                if(i % 2 == 0)
                        printk("\n");
                printk("x%u: %h    ", i, ctx->registers[i]);
        }
        printk("\nesr: %h  far: %h\n", ctx->esr, ctx->far);
        printk("spsr: %h  elr: %h\n\n", ctx->spsr, ctx->elr);

        printk("Not recoverable, hang CPU\n");
        while(1)
                ;
}
void irq_rcver(struct irq_ctx *ctx)
{
        printk("IRQ occured!\n");
        irq_dev->handle_pending();
}
void fiq_rcver(struct irq_ctx *ctx)
{
        printk("FIQ occured!\n");
}
void ser_rcver(struct irq_ctx *ctx)
{
        printk("System Error occured!\n");
        for(int i = 0; i < ARRAY_SZ(ctx->registers); i++)
        {
                if(i % 2 == 0)
                        printk("\n");
                printk("x%u: %h    ", i, ctx->registers[i]);
        }
        printk("\nesr: %h  far: %h\n", ctx->esr, ctx->far);
        printk("spsr: %h  elr: %h\n\n", ctx->spsr, ctx->elr);

        printk("Not recoverable, hang CPU\n");
        while(1)
                ;

}
void ud_rcver(struct irq_ctx *ctx)
{
        printk("An unimplemented interrupt has occured\n");
        for(int i = 0; i < ARRAY_SZ(ctx->registers); i++)
        {
                if(i % 2 == 0)
                        printk("\n");
                printk("x%u: %h    ", i, ctx->registers[i]);
        }
        printk("\nesr: %h  far: %h\n", ctx->esr, ctx->far);
        printk("spsr: %h  elr: %h\n\n", ctx->spsr, ctx->elr);

        printk("Not recoverable, hang CPU\n");
        while(1)
                ;
}
