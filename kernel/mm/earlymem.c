#include <types.h>

#define EARLY_MEM_MAX_REGIONS 32

struct _early_memory_region {
	u64 base;
	u64 size;
};

struct _early_memory_region regions[EARLY_MEM_MAX_REGIONS];
