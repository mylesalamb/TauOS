#ifndef IRQ_H
#define IRQ_H 1

#include <types.h>

struct irq_ctx {
        u64 registers[31];
        u64 esr;
        u64 far;
        u64 spsr;
        u64 elr;
};

void irq_enable();
void irq_disable();
void irq_init_vectors();

void exception_handler(struct irq_ctx *);

#endif
