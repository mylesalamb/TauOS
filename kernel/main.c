#include <pl011.h>

void kinit(void *dtb)
{
	pl011_init(0x09000000);
	pl011_puts("Hello world!\n");
	while (1)
		asm volatile ("nop");
}
