#include <types.h>
#include <gpio.h>
#include <aux.h>
#include <drv/muart.h>
#include <lib/io.h>

#define MUART_FN        gf_alt5
#define MUART_RXD0      15
#define MUART_TXD0      14
#define MUART_ENABLE    (1 << 0)

void delay(u64 n)
{
        for(;n;n--)
                __asm__ __volatile__("nop");
}

void muart_writec(char);
void muart_writes(char *);

struct console muart_console = {
        .writes = muart_writes
};

void muart_init()
{
        gpio_setfunc(MUART_TXD0, MUART_FN);
        gpio_setfunc(MUART_RXD0, MUART_FN);
        gpio_enable(MUART_TXD0);
        gpio_enable(MUART_RXD0);

        struct aux_regs *aux = aux_get();
        aux->AUX_ENABLES = MUART_ENABLE;
        aux->AUX_MU_CNTL_REG = 0;
        aux->AUX_MU_IER_REG = 0;
        aux->AUX_MU_LCR_REG = 3;
        aux->AUX_MU_MCR_REG = 0;
        aux->AUX_MU_BAUD_REG = 541;
        aux->AUX_MU_CNTL_REG = 3;
        /* Needs some number of cpu cycles to become stable */
        delay(350);
        muart_writec('\n');
        muart_writec('\n');
}

void _muart_writec(char chr)
{

        struct aux_regs *aux = aux_get();
        while(!(aux->AUX_MU_LSR_REG & (0x20)))
                ;
        aux->AUX_MU_IO_REG = chr;
}

void muart_writec(char chr)
{
        /* preserve proper behaviour even if we are writing a single char */
        if(chr == '\n')
                _muart_writec('\r');
        _muart_writec(chr);
}

void muart_writes(char *str)
{
        while(*str) {
                muart_writec(*str);
                str++;
        }
}

char muart_readc()
{
        struct aux_regs *aux = aux_get();
        while(!(aux->AUX_MU_LSR_REG & 1))
                ;
        return aux->AUX_MU_IO_REG & 0xFF;
}

