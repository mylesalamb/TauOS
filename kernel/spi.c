#include <aux.h>
#include <gpio.h>

#define SPI_CS1  7
#define SPI_CS0  8
#define SPI_MISO 9
#define SPI_MOSI 10
#define SPI_SCLK 11

#define SPI_ENABLE 0x02

void spi_init() 
{
        struct aux_regs *regs;

        gpio_setfunc(SPI_CS1, gf_alt0);
        gpio_setfunc(SPI_CS0, gf_alt0);
        gpio_setfunc(SPI_MISO, gf_alt0);
        gpio_setfunc(SPI_MOSI, gf_alt0);
        gpio_setfunc(SPI_SCLK, gf_alt0);

        gpio_enable(SPI_CS1);
        gpio_enable(SPI_CS0);
        gpio_enable(SPI_MISO);
        gpio_enable(SPI_MOSI);
        gpio_enable(SPI_SCLK);

        regs = aux_get();
        regs->AUX_ENABLES |= SPI_ENABLE;
}
