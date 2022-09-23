#ifndef IRQ_H
#define IRQ_H 1

#include <types.h>

struct irq_interface {
       
        void (*init)();
        void (*handle_pending)();
        void (*install_handler)(u32, void (*)());      
};

struct irq_ctx {
        u64 registers[31];
        u64 esr;
        u64 far;
        u64 spsr;
        u64 elr;
};

void irq_init(struct irq_interface*, u64);
void irq_enable();
void irq_disable();
void irq_init_vectors();
void irq_install_handler(u32, void (*)());

void se_rcver(struct irq_ctx *);
void irq_rcver(struct irq_ctx *);
void fiq_rcver(struct irq_ctx *);
void ser_rcver(struct irq_ctx *);
void ud_rcver(struct irq_ctx *);
#endif
