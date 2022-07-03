#ifndef MUART_H
#define MUART_H 1

#include <lib/io.h>

extern struct console muart_console;

void muart_init();
void muart_writes(char*);
void muart_writec(char);

#endif
