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

	/* For each source entry, push to the buddy allocator that it is available */
	earlymem_source_len(&l);
	for (size_t i = 0; i < l; i++) {
		earlymem_source_ent(i, &b, &s, &f);
		printk("Printing source entry [%lx, %lx)\n", b, s + b);
	}

	/* For each used entry, push to the buddy allocator that it is used */
	earlymem_used_len(&l);
	printk("Have %d entires to add\n", l);
	for (size_t i = 0; i < l; i++) {
		earlymem_used_ent(i, &b, &s, &f);
		printk("Printing used entry [%lx, %lx)\n", b, s + b);
	}
	earlymem_available_len(&l);
	printk("Have %d entires to add\n", l);
	for (size_t i = 0; i < l; i++) {
		earlymem_available_ent(i, &b, &s, &f);
		printk("Printing available entry [%lx, %lx)\n", b, s + b);
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
