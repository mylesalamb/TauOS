#ifndef TRACE_H
#define TRACE_H 1

#include <printk.h>

/**
    Generic macros for expansion on whether certain pre-processor variables are set
*/

#define TRACE_IF_ENABLED_1(fmt, ...) \
    do { \
        printk("[%s:%d] " fmt, \
               __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define TRACE_IF_ENABLED_0(fmt, ...) \
    do { } while (0)

#define TRACE_IF_ENABLED(x) TRACE_IF_ENABLED_##x

#define _trace(cat, ...) \
    TRACE_IF_ENABLED(cat)(__VA_ARGS__)

#endif
