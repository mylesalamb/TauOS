#include <types.h>
#include <irq.h>
#include <lib/io.h>
#include <lib/common.h>

void exception_handler(struct irq_ctx *ctx)
{
        printk("An exception has occured!\n");
        for(int i = 0; i < ARRAY_SZ(ctx->registers); i++)
        {
                if(i % 2 == 0)
                        printk("\n");
                printk("x%u: %h    ", i, ctx->registers[i]);
        }
        printk("\nesr: %h  far: %h\n", ctx->esr, ctx->far);
        printk("spsr: %h  elr: %h\n\n", ctx->spsr, ctx->elr);

        printk("Exception is not recoverable, hang CPU\n");
        while(1)
                ;
}
