#include <stddef.h>
#include <printk.h>
#include <types.h>
#include <mm/pt.h>
#include <mm/addr.h>
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

int _palloc_is_buddy(struct page *a, struct page *b, uint order)
{
	uintptr_t addr_a = (uintptr_t) memmap_ptoa(a);
	uintptr_t addr_b = (uintptr_t) memmap_ptoa(b);

	size_t block_size = MMU_GRANULE << order;

	// Must differ by exactly one block at this order
	return (addr_a ^ addr_b) == block_size;
}

/**
	Given a struct page split the page into two
*/
int _palloc_split(struct page *p, struct page **b, uint order)
{
	uintptr_t a;
	if (p->order == PALLOC_4KB)
		return -1;

	/* Get the address referenced by the page */
	a = (uintptr_t) memmap_ptoa(p);
	*b = memmap_vaddr((void *)(a ^ (MMU_GRANULE << order)));
	return 0;

}

/**
 * Given a pointer to a memmap page, make it available
 * in the buddy allocator again
 */
int _palloc_insert(struct page *p)
{
	while (p->order + 1 < _PALLOC_LIMIT) {
		struct page **prev = &avail_blocks[p->order];
		struct page *cur = avail_blocks[p->order];

		while (cur) {
			if (_palloc_is_buddy(cur, p, p->order)) {

				*prev = cur->next;

				if (cur < p)
					p = cur;

				p->order++;
				p->next = NULL;

				goto try_next_order;
			}

			prev = &cur->next;
			cur = cur->next;
		}

		break;
 try_next_order:
		;
	}

	struct page **csr = &avail_blocks[p->order];
	while (*csr && *csr < p)
		csr = &(*csr)->next;

	p->next = *csr;
	*csr = p;

	return 0;
}

int _palloc_push_avail(uintptr_t base, size_t s, size_t *c)
{
	int r;
	struct page *p;
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

	printk("Pushing block of size: %d (%x), order %d\n", *c, *c, ord);
	p = memmap_paddr((void *)base);
	p->order = ord;
	r = _palloc_insert(p);
	if (r < 0) {
		return r;
	}
	*c = ord_to_extent(ord);

	return 0;
}

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

    it works out the state of main memory from memmap primitives
*/
int palloc_init()
{
	int r;
	size_t l;
	uintptr_t b;
	size_t s;
	int f;

	/* For each free entry push the entries to the page allocator */
	/* I wanted to do a naive loop over memmap although that is suuuper slow */
	earlymem_available_len(&l);
	for (size_t i = 0; i < l; i++) {
		earlymem_available_ent(i, &b, &s, &f);
		r = palloc_push_avail(b, s);
		if (r < 0) {
			return r;
		}
	}

	return 0;
}

void palloc_dump(void)
{
	for (uint order = 0; order < _PALLOC_LIMIT; order++) {
		size_t block_size = (size_t)MMU_GRANULE << order;
		struct page *p = avail_blocks[order];

		printk("palloc: order %d (%d KB):", order, block_size >> 10);

		if (!p) {
			printk(" empty\n");
			continue;
		}

		printk("\n");

		while (p) {
			printk("  - %p (order=%d)", p, p->order);

			if (p->order != order)
				printk(" [CORRUPT]");

			printk("\n");

			p = p->next;
		}
	}
}

int palloc(void **d, uint o)
{
	struct page *p;
	uint order;

	if (!d || o >= _PALLOC_LIMIT)
		return -1;

	/* Find smallest available order >= o */
	for (order = o; order < _PALLOC_LIMIT; order++) {
		if (avail_blocks[order])
			break;
	}

	if (order == _PALLOC_LIMIT)
		return -1;	/* Out of memory */

	/* Remove block from free list */
	p = avail_blocks[order];
	avail_blocks[order] = p->next;
	p->next = NULL;

	/* Split down to requested order */
	while (order > o) {
		order--;

		struct page *buddy;
		_palloc_split(p, &buddy, order);

		buddy->order = order;
		buddy->next = NULL;

		_palloc_insert(buddy);

		p->order = order;
	}

	*d = va(memmap_ptoa(p));
	return 0;
}

int pfree(void *d)
{
	struct page *p = memmap_vaddr(d);
	return _palloc_insert(p);
}
