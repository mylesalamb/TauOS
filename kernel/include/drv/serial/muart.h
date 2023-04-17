#ifndef MUART_H
#define MUART_H 1

#include <lib/io.h>

extern struct console qemu_uart_console;

void muart_init();
void muart_writes(char*);
void muart_writec(char);
char muart_readc();
#endif
