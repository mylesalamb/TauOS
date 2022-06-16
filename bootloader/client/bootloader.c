#include <types.h>
#include <aux.h>
#include <gpio.h>
#include <drv/muart.h>

#define PHYS_BASE_ADDR 0xFE000000
#define KERNEL_START 0x80000

#define BRK '\x03'
#define ACK '\x06'
#define FIN '\x04'

void bmain()
{
        aux_init(PHYS_BASE_ADDR);
        gpio_init(PHYS_BASE_ADDR);
        muart_init();
        muart_writes("[BOOT] Requesting size from host!\n");
        muart_writec(BRK);
        muart_writec(BRK);
        muart_writec(BRK);

        u64 sz = 0;
        for(int i = 0; i < sizeof(u64); i++)
        {
                char c = muart_readc();
                sz |= (c << (i * 8));
        }

        muart_writec(ACK);

        volatile char *dest = (char *)KERNEL_START;
        while(sz--)
        {
                *dest = muart_readc();
                dest++;
        }

        char c;
        while( (c = muart_readc()) != FIN )
        {
                muart_writes("Missed fin\n");
        }

        muart_writes("[BOOT] Copy done, jump to loaded, kernel\n");
}

void bother()
{
        muart_writes("Write some other stuff\n");
}
