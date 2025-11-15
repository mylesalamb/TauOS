#include <stddef.h>
#include <printk.h>
#include <mm/mmu.h>

#define PG_TABLE_ENTRIES 512

#define addr_to_index(a, s) ( ((a) >> s) & MMU_PTE_MASK)
#define entry_to_table(e) ((pg_table *)(e & ~MMU_DCR_ATTR_MASK))
#define pxx_to_mask(e) ((1 << e) - 1)
#define pxx_to_extent(e) ((1 << e))

void _mmu_dump(pg_table *pgd, int l)
{
	printk("Page table entries at: %p, level: %d\n", pgd, l);
	for (int i = 0; i < PG_TABLE_ENTRIES; i++) {
		pg_table e = pgd[i];
		if (e) {
			printk("ad: %x, hx: %x  dc: %d: %lx\n", &pgd[i], i, i,
			       e);
		}
	}

	if (l == 3)
		return;

	for (int i = 0; i < PG_TABLE_ENTRIES; i++) {
		pg_table e = pgd[i];
		if (e & MMU_DCR_TABLE) {
			_mmu_dump(entry_to_table(e), l + 1);
		}
	}
}

void mmu_dump(pg_table *pgd)
{
	printk("Dumping page table rooted at: %p\n", pgd);
	_mmu_dump(pgd, 0);
	printk("Done\n");
}

pg_table _mmu_flags_to_attr(const int flags)
{
	pg_table r = 0;

	if (flags & MMU_MAP_KERNEL_CODE) {
		r |= MMU_DCR_UXN | MMU_DCR_AF | MMU_DCR_ATTR_AP_RW_N;
	}

	if (flags & MMU_MAP_KERNEL_DATA) {
		r |= MMU_DCR_UXN |
		    MMU_DCR_PXN | MMU_DCR_ATTR_AP_RW_N | MMU_DCR_AF;
	}

	if (flags & MMU_MAP_KERNEL_RO) {
		r |= MMU_DCR_UXN |
		    MMU_DCR_PXN | MMU_DCR_ATTR_AP_RO_N | MMU_DCR_AF;
	}

	if (flags & MMU_MAP_DEVICE) {
		r |= MMU_DCR_AF |
		    MMU_DCR_UXN | MMU_DCR_PXN | MMU_DCR_ATTR_DEVICE;
	}

	return r;
}

/* Non allocating function to identity map a region of memory */
int _mmu_early_map_range(pg_table *pgd, void *const s, size_t *c,
			 const int flags)
{
	uintptr_t a;
	pg_table *csr, *e, tmp;
	a = (uintptr_t) s;

	csr = pgd;
	e = &csr[addr_to_index(a, MMU_PGD_SHIFT)];
	if (!(*e & MMU_DCR_VALID)) {
		printk("PGD not mapped, and cannot alloc\n");
		return -1;
	}

	csr = entry_to_table(*e);
	e = &csr[addr_to_index(a, MMU_PUD_SHIFT)];
	/* Not mapped so write as a block mapping */
	if (!(*e & MMU_DCR_VALID)) {
		tmp = _mmu_flags_to_attr(flags) | MMU_DCR_BLOCK | MMU_DCR_VALID;
		tmp |= (uintptr_t) s & ~pxx_to_mask(MMU_PUD_SHIFT);
		*e = tmp;
		*c = pxx_to_extent(MMU_PUD_SHIFT);
		return 0;
	}
	/* Already a block mapping */
	if (!(*e & MMU_DCR_TABLE)) {
		*c = pxx_to_extent(MMU_PUD_SHIFT);
		return 0;
	}

	csr = entry_to_table(*e);
	e = &csr[addr_to_index(a, MMU_PMD_SHIFT)];

	if (!(*e & MMU_DCR_VALID)) {
		tmp = _mmu_flags_to_attr(flags) | MMU_DCR_BLOCK | MMU_DCR_VALID;
		tmp |= (uintptr_t) s & ~pxx_to_mask(MMU_PMD_SHIFT);
		*e = tmp;
		*c = pxx_to_extent(MMU_PMD_SHIFT);
		return 0;
	}
	if (!(*e & MMU_DCR_TABLE)) {
		*c = pxx_to_extent(MMU_PMD_SHIFT);
		return 0;
	}

	csr = entry_to_table(*e);
	e = &csr[addr_to_index(a, MMU_PTE_SHIFT)];
	if (!(*e & MMU_DCR_VALID)) {
		tmp = _mmu_flags_to_attr(flags) | MMU_DCR_PAGE | MMU_DCR_VALID;
		tmp |= (uintptr_t) s & ~pxx_to_mask(MMU_PTE_SHIFT);
		*e = tmp;
		*c = pxx_to_extent(MMU_PTE_SHIFT);
		return 0;
	}

	/* Already mapped as a PTE */
	*c = pxx_to_extent(MMU_PTE_SHIFT);
	return 0;
}

/**
	Map a region of memory given by @s and a size of @l

	maps at the least the region enclosed by s + l without allocating
	any additional memory, assumes that no other memory allocators
	are available

	returns 0 on success and <0 on failure
*/
int mmu_early_map_range(pg_table *pgd, void *const s, const size_t l,
			const int flags)
{

	int r;
	size_t c;
	uintptr_t csr;

	csr = (uintptr_t) s;
	do {
		r = _mmu_early_map_range(pgd, (void *)csr, &c, flags);
		if (r < 0)
			return r;

		csr += c;

	} while ((uintptr_t) csr < (uintptr_t) s + l);

	asm volatile ("dsb sy");
	return r;
}

/**
	Asserts whether we can block map a chunk of memory
	if both the source address is aligned to the block and the length
	is greater or equal to the size of the block
*/
int can_block_map(uint s, uintptr_t b, size_t l)
{
	return (!((pxx_to_extent(s) - 1) & b)) && (pxx_to_extent(s) <= l);
}

int _mmu_init_fwd_table(pg_table *entry, const int flags,
			int (alloc) (pg_table **))
{
	int r;
	pg_table *fwd, tmp;

	r = alloc(&fwd);
	if (r < 0) {
		return r;
	}

	tmp = _mmu_flags_to_attr(flags) | MMU_DCR_TABLE | MMU_DCR_VALID;
	tmp |= (uintptr_t) fwd;

	*entry = tmp;
	return 0;
}

/**
	Given a pointer to a page table entry marked as a block map
	break the block map into a page table, in the lower level
	block mapping each of its entries with the same flags as the original
	descriptor
*/
int _mmu_push_block(pg_table *entry, unsigned int shift,
		    int (alloc) (pg_table **))
{
	int r;
	pg_table flags, *fwd;
	uintptr_t dest;
	size_t step;

	flags = *entry & MMU_DCR_ATTR_MASK;
	step = pxx_to_extent(shift);

	r = alloc(&fwd);
	if (r < 0)
		return r;

	dest = (uintptr_t) entry_to_table(*entry);
	printk("Extracted dest to push down to %x\n", dest);
	for (int i = 0; i < MMU_BLK_LEN; i++) {
		fwd[i] =
		    flags | dest | ((shift == MMU_PTE_SHIFT) ? (MMU_DCR_PAGE)
				    : (MMU_DCR_BLOCK));
		dest += step;
	}

	/* Rewrite the source entry to refer to the table we made beneath it */
	*entry = flags | MMU_DCR_TABLE | (uintptr_t) fwd;

	return 0;
}

void _mmu_init_block_map(pg_table *e, void *const s, const int f,
			 const int shift)
{
	pg_table tmp;
	tmp = _mmu_flags_to_attr(f) | MMU_DCR_BLOCK | MMU_DCR_VALID;
	tmp |= (uintptr_t) s & ~pxx_to_mask(shift);
	*e = tmp;
}

int _mmu_map_range(pg_table *pgd, void *const s, const size_t l,
		   size_t *const c, const int flags, int (alloc) (pg_table **))
{
	int r;
	uintptr_t a;
	pg_table *csr, *e, tmp;
	a = (uintptr_t) s;

	printk("[*] Mapping range from: %p %lx\n", s, l);

	csr = pgd;
	e = &csr[addr_to_index(a, MMU_PGD_SHIFT)];
	if (!*e) {
		r = _mmu_init_fwd_table(e, flags, alloc);
		if (r < 0)
			return r;
		*c = 0;
		return 0;
	}

	csr = entry_to_table(*e);
	e = &csr[addr_to_index(a, MMU_PUD_SHIFT)];
	if (!*e) {

		if (can_block_map(MMU_PUD_SHIFT, (uintptr_t) s, l)) {
			printk("Block mapping PUD entry\n");
			_mmu_init_block_map(e, s, flags, MMU_PUD_SHIFT);
			*c = pxx_to_extent(MMU_PUD_SHIFT);
			return 0;
		}

		printk("Cannot block map PUD, creating a table\n");

		r = _mmu_init_fwd_table(e, flags, alloc);
		if (r < 0)
			return r;
		*c = 0;
		return 0;
	}
	if (!(*e & MMU_DCR_TABLE)) {

		/* Is it actually already mapped as what we want */
		if ((*e & MMU_DCR_ATTR_MASK & ~MMU_DCR_VALID) ==
		    _mmu_flags_to_attr(flags)) {
			*c = pxx_to_extent(MMU_PUD_SHIFT);
			return 0;
		}

		/* Rewrite block entry if we can */
		if (can_block_map(MMU_PUD_SHIFT, (uintptr_t) s, l)) {
			printk("Block mapping PUD entry\n");
			_mmu_init_block_map(e, s, flags, MMU_PUD_SHIFT);
			*c = pxx_to_extent(MMU_PUD_SHIFT);
			return 0;
		}

		/* Push the block mapping down */
		r = _mmu_push_block(e, MMU_PMD_SHIFT, alloc);
		if (r < 0)
			return r;

		*c = 0;
		return 0;
	}

	csr = entry_to_table(*e);
	e = &csr[addr_to_index(a, MMU_PMD_SHIFT)];
	if (!*e) {

		if (can_block_map(MMU_PMD_SHIFT, (uintptr_t) s, l)) {
			printk("Block mapping PMD entry\n");
			_mmu_init_block_map(e, s, flags, MMU_PMD_SHIFT);
			*c = pxx_to_extent(MMU_PMD_SHIFT);
			return 0;
		}

		printk("Cannot block map PMD, creating a table\n");

		r = _mmu_init_fwd_table(e, flags, alloc);
		if (r < 0)
			return r;
		*c = 0;
		return 0;
	}
	if (!(*e & MMU_DCR_TABLE)) {

		/* Is it actually already mapped as what we want */
		if ((*e & MMU_DCR_ATTR_MASK & ~MMU_DCR_VALID) ==
		    _mmu_flags_to_attr(flags)) {
			*c = pxx_to_extent(MMU_PMD_SHIFT);
			return 0;
		}

		/* Rewrite block entry if we can */
		if (can_block_map(MMU_PMD_SHIFT, (uintptr_t) s, l)) {
			printk("Block mapping PUD entry\n");
			_mmu_init_block_map(e, s, flags, MMU_PMD_SHIFT);
			*c = pxx_to_extent(MMU_PMD_SHIFT);
			return 0;
		}

		/* Push the block mapping down */
		printk("Pushing down block map %x to pte\n", *e);
		r = _mmu_push_block(e, MMU_PTE_SHIFT, alloc);
		if (r < 0)
			return r;

		*c = 0;
		return 0;
	}

	csr = entry_to_table(*e);
	e = &csr[addr_to_index(a, MMU_PTE_SHIFT)];

	tmp = _mmu_flags_to_attr(flags) | MMU_DCR_PAGE | MMU_DCR_VALID;
	tmp |= (uintptr_t) s & ~pxx_to_mask(MMU_PTE_SHIFT);
	*e = tmp;

	*c = pxx_to_extent(MMU_PTE_SHIFT);
	return 0;
}

/**
	Generic mmu mapping function

	Allocates new page tables from @alloc
*/
int mmu_map_range(pg_table *pgd, void *const s, const size_t l, const int flags,
		  int (alloc) (pg_table **))
{

	int r;
	size_t c;
	uintptr_t csr, end;

	end = (uintptr_t) s + l;

	csr = (uintptr_t) s;
	do {
		r = _mmu_map_range(pgd, (void *)csr, end - csr, &c, flags,
				   alloc);
		if (r < 0)
			return r;

		csr += c;

	} while ((uintptr_t) csr < end);

	asm volatile ("dsb sy");
	return 0;
}
