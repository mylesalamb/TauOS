#ifndef PL011_H
#define PL011_H 1

#include <lib/io.h>

extern struct console pl011_console;

void pl011_init();
void pl011_writes(char*);
void pl011_writec(char);
char pl011_readc();
#endif
