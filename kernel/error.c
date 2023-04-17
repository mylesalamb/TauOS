#include <types.h>
#include <lib/io.h>

void _panic(const char *func, const u32 line, const char *fmt, ...)
{
    printk("Kernel panic: %s:%d\n", func, line);
    va_list va;
    va_start(va, fmt);
    vprintk(fmt, va);
    va_end(va);


    /* TODO: unwind the stack, printing out call stack */

    while(1)
        ;
}