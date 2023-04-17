#include <types.h>
#include <lib/io.h>
#include <drv/common.h>

struct pl011_regs {
        ureg32 DR;
        ureg32 RSRECR;
        ureg32 _reserved0[4];
        ureg32 FR;
        ureg32 _reserved1;
        ureg32 ILPR;
        ureg32 IBRD;
        ureg32 FBRD;
        ureg32 LCRH;
        ureg32 CR;
};

struct pl011_regs *pl011_dev = (struct pl011_regs *)0x09000000;


void pl011_init()
{
        /* Nothing to do */
        return;
}

void pl011_writes(char *str)
{
        while(*str) {
                pl011_writec(*str);
                str++;
        }
}

void pl011_writec(char c)
{
        pl011_dev->DR = c;
}

char pl011_readc()
{
        return 'a';
}

struct tauos_device_compat pl011_compat[] = {
        { .compatible = "arm,pl011" },
        { /* Sentinel */}
};

TAU_DRIVER(bcm_muart) = {
       .name = "ARM PL011 serial driver",
       .dclass = DCLASS_SERIAL,
       .compat = pl011_compat
};
