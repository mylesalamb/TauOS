#include <printk.h>
#include <types.h>
#include <mm/palloc.h>
#include <mm/earlymem.h>

/* Bits to shift by from order to extent */
#define PALLOC_ORD_SHIFT (12)

/* Stores an entry for an address that is available */
struct page_descr {
	uintptr_t address;
	struct page_descr *next;
};

struct page_descr *avail_blocks[_PALLOC_LIMIT];

int _palloc_push_avail(uintptr_t base, size_t s)
{

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
		r = _palloc_push_avail(b, s);
		if (r < 0) {
			return r;
		}
	}
	return 0;
}

int palloc(void **d, unsigned int o)
{
	return 0;
}

int pfree(void *d)
{
	return 0;
}
