#include <types.h>
#include <error.h>
#include <printk.h>

struct exc_ctx {
	u64 registers[31];
	u64 esr;
	u64 far;
	u64 spsr;
	u64 elr;
};
/* Exception handlers called from exc.S */
void exc_recv_se(struct exc_ctx *c)
{
	panic("Triggered sync exception");
}

void exc_recv_ser(struct exc_ctx *c)
{
	panic("Triggered system error");
}

void exc_recv_irq(struct exc_ctx *c)
{
	panic("Triggered irq");
}

void exc_recv_fiq(struct exc_ctx *c)
{
	panic("Triggered fiq");
}

void exc_recv_udf(struct exc_ctx *c)
{
	panic("Triggered undefined exception handler");
}
