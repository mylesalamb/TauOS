#ifndef LIB_COMMON
#define LIB_COMMON 1

#include <stddef.h>

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

#define DUMP_OFFSET(s, m) printk("%s %h\n", STRINGIFY(s)"."STRINGIFY(m) ":", offsetof(s,m));
#define ARRAY_SZ(x) (sizeof(x)/sizeof(x[0]))

#define BIT(x) (1l << x)

#define btlhw(x) (( (x & 0x000000ff ) << 24 ) | ( (x & 0x0000ff00 ) << 8 )  | ( (x & 0x00ff0000 ) >> 8 ) | ( (x & 0xff000000 ) >> 24 ) )
#define btlw(x) ((x << 56) | ((x & 0x000000000000FF00) << 40) | ((x & 0x0000000000FF0000) << 24) |((x & 0x00000000FF000000) << 8) |((x & 0x000000FF00000000) >> 8) |((x & 0x0000FF0000000000) >> 24)|((x & 0x00FF000000000000) >> 40)|(x >> 56))


#endif
