#ifndef MM_EARLYMEM_H
#define MM_EARLYMEM_H 1

#include <stddef.h>
#include <types.h>

enum {
	EARLYMEM_CODE = 1,
	EARLYMEM_DATA,
	EARLYMEM_RO,
	EARLYMEM_RESERVED,
};

int earlymem_add_avail(uintptr_t, size_t);
int earlymem_add_used(uintptr_t, size_t, int);
int earlymem_alloc(size_t, size_t, void **);

void earlymem_source_len(size_t *);
void earlymem_used_len(size_t *);
void earlymem_available_len(size_t *);

int earlymem_used_ent(size_t, uintptr_t *, size_t *, int *);
int earlymem_source_ent(size_t, uintptr_t *, size_t *, int *);
int earlymem_available_ent(size_t, uintptr_t *, size_t *, int *);

#endif
