#ifndef MM_EARLYMEM_H
#define MM_EARLYMEM_H 1

#include <stddef.h>
#include <types.h>

int earlymem_add_avail(u64, u64);
int earlymem_add_used(u64, u64);
int earlymem_alloc(size_t, void **);

#endif
