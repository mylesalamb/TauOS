#ifndef MM_ADDR_H
#define MM_ADDR_H 1

/* Base address of the higher half mappings */
#define KVADDR_BASE 0xffff000000000000UL

#define pa(x) ((void *)((uintptr_t)x & ~KVADDR_BASE))
#define va(x) ((void *)((uintptr_t)x | KVADDR_BASE))

#endif
