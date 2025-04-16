#include <stdint.h>
#include <types.h>
#include <mmio.h>

enum {
	UARTDR = 0x00,
	UARTFR = 0x18,
	UARTCR = 0x30,
};

uintptr_t uart_base = 0;

void pl011_init(uintptr_t address)
{
	uart_base = address;
	u16 cr = mmio_read16(uart_base + UARTCR);
	mmio_write16(uart_base + UARTCR, cr | (1 << 8));
}

void pl011_putc(const char c)
{
	if (c == '\n') {
		mmio_write8(uart_base + UARTDR, '\r');
	}

	mmio_write8(uart_base + UARTDR, c);
}

char pl011_readc()
{
	while (mmio_read16(uart_base + UARTFR) & (1 << 4)) ;

	return mmio_read8(uart_base + UARTDR);
}

void pl011_puts(const char *s)
{
	if (!s)
		return;

	while (*s) {
		pl011_putc(*s++);
	}
}
