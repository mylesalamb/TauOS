#ifndef MEM_H
#define MEM_H 1

#include <stddef.h>
#include <types.h>

/* Mapping from legacy master to arm addresses for lower first gigabyte */
#define BUS_TO_PHYS(x) ((void *)((u64)x & ~0xC0000000))
#define PHYS_TO_BUS(x) ((void *)((u64)x |  0xC0000000))

void memcpy(const void *restrict, void *restrict, size_t);
void memset(void *, u64, size_t);
void memmove(void *, void *, size_t);
void memzero(void *, size_t);

#endif
