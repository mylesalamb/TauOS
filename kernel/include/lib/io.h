#ifndef IO_H
#define IO_H 1

#include <stdarg.h>
#include <types.h>

struct console 
{
        void (*writes)(char *);
};

void io_init(struct console *);
u64 cprintk(struct console *, const char *, ...);
u64 printk(const char *, ...);
u64 sprintf(char *, const char *, ...);

#endif
