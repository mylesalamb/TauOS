#include <stddef.h>
#include <types.h>
#include <lib/fdt.h>
#include <printk.h>
#include <endian.h>
#include <mm/mmu.h>
#include <mm/earlymem.h>
#include <mm/pt.h>

#define MAX_FDT_CELL_SIZE 2

extern char __TEXT_START;
extern char __TEXT_END;

extern char __RODATA_START;
extern char __RODATA_END;

extern char __DATA_START;
extern char __DATA_END;

extern char __BSS_START;
extern char __BSS_END;

extern char __IDMAP_DATA_START;
extern char __IDMAP_DATA_END;

extern pg_table __idmap_blocks;

char *cmdline;
char cmdline_present;

void *initrd;
size_t initrd_size;

int plat_early_scan_memory(const struct fdt_header *h);
int plat_early_scan_chosen(const struct fdt_header *h);

/**
	plat_early_scan:

	Scans configuration provided by either firmware or platform specific firmware,
	this function initialises the earlymem boot time allocator with memory regions
	known by the device tree.

	returns, 0 on success <0 otherwise
*/
int plat_early_scan(const struct fdt_header *h)
{
	int r;

	r = mmu_early_map_range(&__idmap_blocks, (void *)h, FDT_MAX_SIZE,
				MMU_MAP_KERNEL_DATA);
	if (r < 0) {
		return -1;
	}
	r = fdt_check_header(h);
	if (r < 0) {
		return -1;
	}

	r = plat_early_scan_memory(h);
	if (r < 0) {
		return -1;
	}

	r = plat_early_scan_chosen(h);
	if (r < 0) {
		return -1;
	}

	return 0;
}

/**
    plat_early_scan_memory:

	initialize the memory mapping of the kernel, bootstrapping
	the earlymem memory manager
*/
int plat_early_scan_memory(const struct fdt_header *h)
{
	int r;
	uint offset, parent;
	int addr_cells, sz_cells;
	const char *memory = "/memory";
	u32 *b, l;

	r = fdt_nodepath_offset(h, memory, &offset);
	if (r < 0)
		return r;

	r = fdt_nodeparent(h, offset, &parent);
	if (r < 0)
		return r;

	r = fdt_getprop(h, offset, "reg", (void **)&b, &l);
	if (r < 0)
		return r;

	addr_cells = fdt_address_cells(h, parent);
	sz_cells = fdt_size_cells(h, parent);

	if (addr_cells < 0) {
		return addr_cells;
	}

	if (sz_cells < 0) {
		return sz_cells;
	}

	if (addr_cells > MAX_FDT_CELL_SIZE) {
		return -1;
	}

	if (sz_cells > MAX_FDT_CELL_SIZE) {
		return -1;
	}

	for (size_t i = 0; i < l / (sizeof(u32) * sz_cells * addr_cells); i++) {
		u64 address = 0;
		u64 size = 0;
		for (int j = 0; j < addr_cells; j++) {
			address = (address << 32) | beth32(b++);
		}

		for (int j = 0; j < sz_cells; j++) {
			size = (size << 32) | beth32(b++);
		}

		r = earlymem_add_avail(address, size);
		if (r < 0)
			return r;
	}

	earlymem_add_used((uintptr_t) & __TEXT_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __TEXT_END -
					    (uintptr_t) & __TEXT_START));
	earlymem_add_used((uintptr_t) & __RODATA_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __RODATA_END -
					    (uintptr_t) & __RODATA_START));
	earlymem_add_used((uintptr_t) & __DATA_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __DATA_END -
					    (uintptr_t) & __DATA_START));
	earlymem_add_used((uintptr_t) & __BSS_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __BSS_END -
					    (uintptr_t) & __BSS_START));
	earlymem_add_used((uintptr_t) & __IDMAP_DATA_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __IDMAP_DATA_END -
					    (uintptr_t) & __IDMAP_DATA_START));

	return 0;
}

int _plat_get_addr_prop(const struct fdt_header *h, unsigned int offset,
			const char *name, void **v)
{

	u32 l;
	u64 *b;
	int retval;

	retval = fdt_getprop(h, offset, name, (void **)&b, &l);

	if (retval < 0)
		return 0;

	if (l != sizeof(u64)) {
		return -1;
	}
	*v = (void *)beth64(b);
	return 0;
}

int _plat_get_str_prop(const struct fdt_header *h, unsigned int offset,
		       const char *name, char **v)
{
	u32 l;
	return fdt_getprop(h, offset, name, (void **)v, &l);
}

int plat_early_scan_chosen(const struct fdt_header *h)
{
	int r;
	void *start, *end;
	unsigned int offset = 0;
	const char *chosen = "/chosen";
	r = fdt_nodepath_offset(h, chosen, &offset);
	if (r < 0) {
		return r;
	}

	r = _plat_get_addr_prop(h, offset, "linux,initrd-start", &start);
	if (r < 0)
		goto einitrd;

	r = _plat_get_addr_prop(h, offset, "linux,initrd-end", &end);
	if (r < 0)
		goto einitrd;

	r = earlymem_add_used((u64) start,
			      MMU_GRANULE_ALIGN((u64) end - (u64) start));
	if (r < 0)
		return -1;

	initrd = start;
	initrd_size = (uintptr_t) end - (uintptr_t) start;

 einitrd:
	r = _plat_get_str_prop(h, offset, "bootargs", &cmdline);
	if (r < 0)
		return r;
	cmdline_present = 1;

	return 0;
}

int _plat_early_alloc_table(pg_table **p)
{
	int r;

	r = earlymem_alloc(MMU_BLK_SIZE, MMU_BLK_SIZE, (void **)p);
	if (r < 0)
		return r;

	r = mmu_early_map_range(&__idmap_blocks, (void *)*p, MMU_BLK_SIZE,
				MMU_MAP_KERNEL_DATA);

	return r;
}

/**
	plat_init_logical

	Initializes the higher half mapping of the kernel making 0xffff addresses usable

*/
int plat_init_logical()
{
	int r;
	pg_table *pgd;

	r = _plat_early_alloc_table(&pgd);
	if (r < 0)
		return r;

	mmu_map_range(pgd, &__TEXT_START,
		      (uintptr_t) & __TEXT_END - (uintptr_t) & __TEXT_END,
		      MMU_MAP_KERNEL_CODE);
	mmu_map_range(pgd, &__RODATA_START,
		      (uintptr_t) & __RODATA_END - (uintptr_t) & __RODATA_END,
		      MMU_MAP_KERNEL_RO);
	mmu_map_range(pgd, &__DATA_START,
		      (uintptr_t) & __DATA_END - (uintptr_t) & __DATA_END,
		      MMU_MAP_KERNEL_DATA);

	printk("Generate logical ok\n");

	asm volatile ("msr ttbr1_el1, %0; isb"::"r" ((uintptr_t) pgd)
	    );
	return 0;
}
