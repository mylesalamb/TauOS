#ifndef SPI_H
#define SPI_H 1


#include <types.h>

struct spi_regs{
        ureg32 cs;
        ureg32 fifo;
        ureg32 clk;
        ureg32 dlen;
        ureg32 ltoh;
        ureg32 dc;
};

void spi_init();

#endif
