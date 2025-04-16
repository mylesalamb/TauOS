#ifndef PRINTK_H
#define PRINTK_H 1

#include <stdarg.h>

void register_console(void (*)(const char *));
int printk(const char *, ...);
int sprintf(char *, const char *, ...);
#endif
