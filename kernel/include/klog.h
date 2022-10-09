#ifndef KLOG_H
#define KLOG_H 1

#include <lib/io.h>

void klog_init(struct console *);
void _klog_write(const char *, const char*, const u32, const char *, ...);


#define klog_debug(...) _klog_write("debug", __func__, __LINE__, ##__VA_ARGS__)
#define klog_info(...) _klog_write("info", __func__, __LINE__, ##__VA_ARGS__)
#define klog_warn(...) _klog_write("warn", __func__, __LINE__, ##__VA_ARGS__)
#define klog_error(...) _klog_write("error", __func__, __LINE__, ##__VA_ARGS__)

#endif
