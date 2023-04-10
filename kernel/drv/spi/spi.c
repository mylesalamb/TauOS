#include <aux.h>
#include <gpio.h>
#include <lib/common.h>


#define SPI_CS1  7
#define SPI_CS0  8
#define SPI_MISO 9
#define SPI_MOSI 10
#define SPI_SCLK 11

#define SPI_ENABLE 0x02
#define SPI_REGS_OFFSET 0x204000


#define SPI_CS_RXF BIT(20)
#define SPI_CS_RXR BIT(19)
#define SPI_CS_TXD BIT(18)
#define SPI_CS_RXD BIT(17)


struct spi_regs *spi_dev;

void spi_init(u64 base_addr) 
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

        spi_dev = (struct spi_regs *)(base_addr + SPI_REGS_OFFSET);
}

struct spi_regs *spi_get()
{
        return spi_dev;
}


void spi_send(u8 *buff, u64 sz)
{
        struct spi_regs *regs = spi_get();



}
