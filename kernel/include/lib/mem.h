#ifndef MEM_H
#define MEM_H 1

#include <stddef.h>
#include <types.h>

void memcpy(const void *restrict, void *restrict, size_t);
void memset(void *, u64, size_t);
void memmove(void *, void *, size_t);

#endif
