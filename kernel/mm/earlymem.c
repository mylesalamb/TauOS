#include <types.h>
#include <stddef.h>
#include <printk.h>

#define EARLY_MEM_MAX_RESERVED 	32
#define EARLY_MEM_MAX_AVAIL 	32

struct early_memory_region {
	u64 base;
	u64 size;
};

size_t avail_idx;
struct early_memory_region avail_regions[EARLY_MEM_MAX_AVAIL];

size_t used_idx;
struct early_memory_region used_regions[EARLY_MEM_MAX_RESERVED];

int _earlymem_add(struct early_memory_region **a, size_t *i, size_t m, u64 b,
		  u64 s)
{
	if (*i >= m)
		return -1;

	(*a)[*i].base = b;
	(*a)[*i].size = s;

	(*i)++;
	return 0;
}

int earlymem_add_avail(u64 b, u64 s)
{
	printk("advertise memory: %d [%#0lx, %#0lx)\n", avail_idx, b, b + s);
	return _earlymem_add((struct early_memory_region **)&avail_regions,
			     &avail_idx, EARLY_MEM_MAX_AVAIL, b, s);
}

int earlymem_add_used(u64 b, u64 s)
{
	printk("reserve memory: %d [%#0lx, %#0lx)\n", used_idx, b, b + s);
	return _earlymem_add((struct early_memory_region **)&used_regions,
			     &used_idx, EARLY_MEM_MAX_RESERVED, b, s);
}
