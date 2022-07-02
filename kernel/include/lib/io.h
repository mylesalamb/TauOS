#ifndef IO_H
#define IO_H 1

#include <stdarg.h>
#include <types.h>

struct console 
{
        (*writes)(char *);
};

void io_init(void (*)(char));
u64 cprintk( void(*)(char), const char *, ...);
u64 printk(const char *, ...);

#endif
