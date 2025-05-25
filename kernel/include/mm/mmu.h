#ifndef MM_MMU_H
#define MM_MMU_H 1

#include <stddef.h>
#include <types.h>
#include <mm/pt.h>

typedef u64 pg_table;

#define MMU_MAP_KERNEL_CODE  (1 << 0)
#define MMU_MAP_KERNEL_DATA  (1 << 1)
#define MMU_MAP_KERNEL_RO    (1 << 2)
#define MMU_MAP_DEVICE       (1 << 3)

void mmu_dump();
int mmu_early_map_range(pg_table *, void *const, size_t, int);
int mmu_map_range(pg_table *, void *const, const size_t, const int,
		  int (f) (pg_table **));

#endif
