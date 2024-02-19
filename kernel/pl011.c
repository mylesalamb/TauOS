#include <stdint.h>
#include <types.h>
#include <mmio.h>

enum {
	UARTDR = 0x00,
	UARTRSR = 0x04,
	UARTFR = 0x18,
};

uintptr_t uart_base = 0;

void pl011_init(uintptr_t address)
{
	uart_base = address;
}

void pl011_putc(const char c)
{

	mmio_write8(uart_base + UARTDR, c);

}

void pl011_puts(const char *s)
{

	if (!s)
		return;

	while (*s) {
		pl011_putc(*s++);
	}
}
