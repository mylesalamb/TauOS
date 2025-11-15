#include <types.h>
#include <printk.h>
#include <stddef.h>
#include <lib/mem.h>
#include <mm/pt.h>
#include <mm/addr.h>
#include <mm/memmap.h>
#include <mm/earlymem.h>

struct page *_memmap_blocks;
uintptr_t _memmap_block_base;
size_t _memmap_block_len;

/**
    memmap_len: 
    
    Gets the number of entries in
    the memmap block
*/
size_t memmap_len()
{
	return _memmap_block_len;
}

/**
	Given a pointer to a struct page,
	return the address to the referenced memory address
*/
void *memmap_ptoa(struct page *p)
{
	size_t i = p - _memmap_blocks;
	return (void *)(_memmap_block_base + (i * MMU_GRANULE));
}

/**
    memmap_entry:
    
    Returns a pointer to the @i'th
    memmap block entry
*/
struct page *memmap_entry(size_t i)
{
	return _memmap_blocks + i;
}

/**
    memmap_paddr:

    given a physical address in @paddr
    returns a pointer to the corresponding
    memmap block entry
*/
struct page *memmap_paddr(void *paddr)
{
	uintptr_t p = (uintptr_t) paddr;

	p -= _memmap_block_base;
	p /= MMU_GRANULE;

	return _memmap_blocks + p;
}

/**
 * Given a virtual address, return a reference to the struct
 * page that refers to this address
 */
struct page *memmap_vaddr(void *vaddr)
{
	void *phys = pa(vaddr);
	return memmap_paddr(phys);
}

void _memmap_mark_range(uintptr_t b, size_t s, uint flags)
{
	struct page *csr = memmap_paddr((void *)b);
	size_t cnt = s / MMU_GRANULE;

	printk("Marking %p + %d with %d\n", csr, cnt, flags);

	for (size_t i = 0; i < cnt; i++) {
		csr[i].flags = flags;
	}
}

int _memmap_populate()
{
	size_t l;

	uintptr_t b;
	size_t s;
	int f;

	/* For each used entry, mark it as so */
	earlymem_used_len(&l);
	printk("Have %d entires to add\n", l);
	for (size_t i = 0; i < l; i++) {
		earlymem_used_ent(i, &b, &s, &f);
		printk("Pushing used entry [%lx, %lx)\n", b, s + b);
		_memmap_mark_range(b, s, MEMMAPF_KRES);
	}

	earlymem_available_len(&l);
	printk("Have %d entires to add\n", l);
	for (size_t i = 0; i < l; i++) {
		earlymem_available_ent(i, &b, &s, &f);
		printk("Pushing available entry [%lx, %lx)\n", b, s + b);
		_memmap_mark_range(b, s, MEMMAPF_FREE);
	}
	return 0;
}

int memmap_init()
{
	int r, f;
	uintptr_t low, high;
	size_t l, s, extent, entries;

	earlymem_source_len(&l);

	/* Defensive probably unrealistic */
	if (!l)
		return -1;

	r = earlymem_source_ent(0, &low, &s, &f);
	if (r < 0)
		return r;

	r = earlymem_source_ent(l - 1, &high, &s, &f);
	if (r < 0)
		return r;

	high += s;
	extent = high - low;
	entries = extent / MMU_GRANULE;

	/* Assumption that memory base + limit is 4kb aligned */
	printk("Memory spans [%lx, %lx], len = %lx\n", low, high, extent);
	printk("Need %d entries for memmap\n", extent / MMU_GRANULE);

	r = earlymem_alloc(entries * sizeof(struct page), MMU_GRANULE,
			   (void **)&_memmap_blocks);
	if (r < 0) {
		return r;
	}
	_memmap_blocks = (struct page *)va(_memmap_blocks);
	_memmap_block_len = entries;
	_memmap_block_base = low;

	printk("memmap rooted at: %p\n", _memmap_blocks);

	memset(_memmap_blocks, 0, entries * sizeof(struct page));
	r = _memmap_populate();
	if (r < 0) {
		return r;
	}

	return 0;
}
