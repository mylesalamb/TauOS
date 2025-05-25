#include <stddef.h>
#include <printk.h>
#include <mm/mmu.h>

#define PG_TABLE_ENTRIES 512

#define addr_to_index(a, s) ( ((a) >> s) & MMU_PTE_MASK)
#define entry_to_table(e) ((pg_table *)(e & ~MMU_DCR_ATTR_MASK))
#define pxx_to_mask(e) ((1 << e) - 1)
#define pxx_to_extent(e) ((1 << e))

void mmu_dump(pg_table *pgd)
{
	printk("Dumping page table entries at: %p\n", pgd);

	for (int i = 0; i < PG_TABLE_ENTRIES * 12; i++) {
		pg_table e = pgd[i];
		if (e) {
			printk("%x %d: %lx\n", &pgd[i], i, e);
		}
	}
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
	if (!(*e & MMU_DCR_VALID)) {
		tmp = _mmu_flags_to_attr(flags) | MMU_DCR_BLOCK | MMU_DCR_VALID;
		tmp |= (uintptr_t) s & ~pxx_to_mask(MMU_PUD_SHIFT);
		*e = tmp;
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

	csr = entry_to_table(*e);
	e = &csr[addr_to_index(a, MMU_PTE_SHIFT)];
	if (!(*e & MMU_DCR_VALID)) {
		tmp = _mmu_flags_to_attr(flags) | MMU_DCR_PAGE | MMU_DCR_VALID;
		tmp |= (uintptr_t) s & ~pxx_to_mask(MMU_PTE_SHIFT);
		*e = tmp;
		*c = pxx_to_extent(MMU_PTE_SHIFT);
		return 0;
	}

	return 0;
}

/**
	Map a region of memory given by @s and a size of @l

	maps at the least the region enclosed by s + l without allocating
	any additional memory

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
			break;

		csr += c;

	} while ((uintptr_t) csr < (uintptr_t) s + l);

	asm volatile ("dsb sy");
	return r;
}
