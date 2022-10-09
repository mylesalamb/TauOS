#include <lib/io.h>
#include <stdarg.h>

struct console *log_dev;

void klog_init(struct console *dev)
{
        log_dev = dev; 
}
void _klog_write(const char *level, const char *func, const u32 line, const char *fmt, ...)
{
        cprintk(log_dev, "[%s] %s::%d > ", level, func, line);
        va_list va;
        va_start(va, fmt);
        vcprintk(log_dev, fmt, va);
        va_end(va);
}
