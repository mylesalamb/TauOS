#include <stddef.h>
#include <printk.h>
#include <types.h>
#include <mm/palloc.h>
#include <mm/memmap.h>
#include <mm/earlymem.h>

/* Bits to shift by from order to extent */
#define PALLOC_ORD_SHIFT (12)
#define PALLOC_MASK ((1 << (PALLOC_ORD_SHIFT)) - 1)

/* Stores an entry for an address that is available */

struct page *avail_blocks[_PALLOC_LIMIT];

/**
	Given an address, compute the largest order that address
	could be resident in
*/
uint max_ord(uintptr_t v)
{
	if (!v)
		return PALLOC_4MB;

	int tz = __builtin_ctz(v);
	return tz - PALLOC_ORD_SHIFT;
}

size_t ord_to_extent(uint ord)
{
	return 1 << (ord + 12);
}

uint extent_to_ord(size_t s)
{
	return __builtin_ctz(s) - PALLOC_ORD_SHIFT;
}

int _palloc_insert(uintptr_t b, uint o)
{

	return 0;
}

int _palloc_push_avail(uintptr_t base, size_t s, size_t *c)
{
	/* Sanity check, is the block 4kb aligned, and at least that size */
	if (base & PALLOC_MASK || s & PALLOC_MASK
	    || s < ord_to_extent(PALLOC_4KB))
		return -1;

	uint ord;
	uint addr_ord = max_ord(base);
	uint ext_ord = extent_to_ord(base);

	printk("address: %x, extent: %x\n", base, s);
	printk("address order: %d, extent order: %d\n", addr_ord, ext_ord);

	ord = (addr_ord > ext_ord) ? ext_ord : addr_ord;

	/* If the block is bigger than what we support */
	if (ord >= _PALLOC_LIMIT)
		ord = _PALLOC_LIMIT - 1;

	*c = ord_to_extent(ord);

	printk("Pushing block of size: %d (%x), order %d\n", *c, *c, ord);

	return 0;
}

/**
	palloc_push_avail - Given a block of memory given by @base and of size @s

	initialise entries in the page allocator, assumption that the 
	@base and @s are both at least 4kb aligned

	returns 0 on success <0 otherwise
*/
int palloc_push_avail(uintptr_t base, size_t s)
{

	int r;
	size_t c;
	uintptr_t end, csr;

	csr = base;
	end = base + s;

	do {
		r = _palloc_push_avail(csr, end - csr, &c);
		if (r < 0) {
			return r;
		}

		csr += c;
	} while (csr < end);

	return 0;
}

/**
    palloc is the kernel's page level allocator
    using a buddy allocator.

    It works out the state of the kernel address space from the boot time
    earlymem memory manager
*/
int palloc_init()
{
	int r;
	size_t l;

	uintptr_t b;
	size_t s;
	int f;

	/* For each free entry push the entries to the page allocator */
	earlymem_available_len(&l);
	printk("Have %d entires to add\n", l);
	for (size_t i = 0; i < l; i++) {
		earlymem_available_ent(i, &b, &s, &f);
		printk("Pushing available memory [%lx, %lx)\n", b, s + b);
		r = palloc_push_avail(b, s);
		if (r < 0) {
			return r;
		}
	}
	return 0;
}

int palloc(void **d, uint o)
{
	return 0;
}

int pfree(void *d, uint o)
{
	return _palloc_insert((uintptr_t) d, o);
}
