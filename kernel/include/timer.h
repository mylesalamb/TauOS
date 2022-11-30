#ifndef TIMER_H
#define TIMER_H 1

#include <types.h>

struct timer_regs {
        ureg32 CS;
        ureg32 CLO;
        ureg32 CHI;
        ureg32 C0;
        ureg32 C1;
        ureg32 C2;
        ureg32 C3;
};

void timer_init(u64 base_addr);

void timer_sleep_ms(u32);
#endif
