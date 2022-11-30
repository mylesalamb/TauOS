#ifndef LIB_COMMON
#define LIB_COMMON 1

#include <stddef.h>

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

#define DUMP_OFFSET(s, m) printk("%s %h\n", STRINGIFY(s)"."STRINGIFY(m) ":", offsetof(s,m));
#define ARRAY_SZ(x) (sizeof(x)/sizeof(x[0]))

#define BIT(x) (1 << x)
#endif
