#include <types.h>
#include <stddef.h>
#include <printk.h>
#include <lib/mem.h>
#define BYTE_ALIGN(a, x)    (((x) + (a) - 1) & ~((a) - 1))
#define EARLY_MEM_MAX_BLOCKS 32

struct early_memory_region {
	u64 base;
	u64 size;
};

struct early_memory_container {
	size_t idx;
	struct early_memory_region regions[EARLY_MEM_MAX_BLOCKS];
};

struct early_memory_container source, available, used;

/**
	stores an earlymem region, whether used or available

	implicit assumption that regions are non overlapping
*/
int _earlymem_push(struct early_memory_container *c, size_t m, u64 b, u64 s)
{
	size_t i = 0;

	/* Are we full */
	if (c->idx >= m)
		return -1;

	for (;;) {

		/* Scanned to the end so push in */
		if (i == c->idx) {
			break;
		}

		/* Is the current entry bigger */
		if (c->regions[i].base > b) {
			/* Shove it forward */
			memmove(&c->regions + i + 1, &c->regions + i,
				sizeof(c->regions[0]) * c->idx - i);
			break;
		}

		i++;
	}

	c->regions[i].base = b;
	c->regions[i].size = s;

	c->idx++;
	return 0;
}

/**
	claims a region of memory from a memory region array
*/
int _earlymem_reclaim(struct early_memory_container *c, size_t m, u64 b, u64 s)
{
	size_t i = 0;
	struct early_memory_region *csr;

	for (;;) {
		if (i == c->idx)
			return -1;
		/* We have found the block */
		if (b >= c->regions[i].base
		    && b + s <= c->regions[i].base + c->regions[i].size) {

			csr = &c->regions[i];
			break;
		}

		i++;
	}

	u64 lower_size = b - csr->base;
	if (!lower_size) {
		/* claim starts from the beginning, rewrite the entry */
		csr->base = b + s;
		csr->size = csr->size - s;
		return 0;
	}

	u64 upper_base = b + s;
	u64 upper_size = csr->size - (upper_base - csr->base);
	if (!upper_size) {
		/* Claim starts from the top re-write the entry */
		csr->size = csr->size - s;
		return 0;
	}

	/* Else, we are somewhere in the middle */
	/* Are we full */
	if (c->idx >= m)
		return -1;

	csr->size = lower_size;
	memmove(csr + 2, csr + 1, sizeof(*csr) * (c->idx - i - 1));
	(csr + 1)->base = upper_base;
	(csr + 1)->size = upper_size;

	c->idx++;
	return 0;
}

int earlymem_add_avail(u64 b, u64 s)
{
	int r;
	printk("advertise memory: %d [%#0lx, %#0lx)\n", available.idx, b,
	       b + s);
	r = _earlymem_push(&source, EARLY_MEM_MAX_BLOCKS, b, s);
	if (r < 0)
		return r;

	return _earlymem_push(&available, EARLY_MEM_MAX_BLOCKS, b, s);
}

int earlymem_add_used(u64 b, u64 s)
{
	int r;
	printk("reserve memory: %d [%#0lx, %#0lx)\n", used.idx, b, b + s);

	r = _earlymem_reclaim(&available, EARLY_MEM_MAX_BLOCKS, b, s);

	if (r < 0)
		return r;

	return _earlymem_push(&used, EARLY_MEM_MAX_BLOCKS, b, s);
}

/**
	earlymem_alloc: Allocates a region of memory of size @l
	with alignment @a returning the address within @d
*/
int earlymem_alloc(size_t l, size_t a, void **d)
{
	int r;
	struct early_memory_region *region;
	size_t c = 0;

	u64 b;

	printk("Start try alloc\n");

	while (c < available.idx) {
		region = &available.regions[c++];

		printk("Checking region [%lx, %lx)\n", region->base,
		       region->base + region->size);

		b = BYTE_ALIGN(a, region->base);

		/* Overflow, fixing to the desired alignment */
		if (b + l > region->base + region->size)
			continue;

		r = _earlymem_reclaim(&available, EARLY_MEM_MAX_BLOCKS, b, l);
		if (r < 0)
			return r;
		r = _earlymem_push(&used, EARLY_MEM_MAX_BLOCKS, b, l);
		if (r < 0)
			return r;

		printk("Found region for use: %lx\n", b);
		*d = (void *)b;
		return 0;

	}

	return -1;
}
