#include <stddef.h>
#include <types.h>
#include <lib/fdt.h>
#include <lib/tar.h>
#include <printk.h>
#include <endian.h>
#include <pl011.h>
#include <mm/mmu.h>
#include <mm/addr.h>
#include <mm/earlymem.h>
#include <mm/pt.h>
#include <lib/mem.h>

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

void *initrd;
size_t initrd_size;

int cmdline_present = 0;
int initrd_present = 0;

pg_table *ktable;

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

	/* Identity map the maximum region that the devicetree can occupy */
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
					    (uintptr_t) & __TEXT_START),
			  EARLYMEM_CODE);
	earlymem_add_used((uintptr_t) & __RODATA_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __RODATA_END -
					    (uintptr_t) & __RODATA_START),
			  EARLYMEM_RO);
	earlymem_add_used((uintptr_t) & __DATA_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __DATA_END -
					    (uintptr_t) & __DATA_START),
			  EARLYMEM_DATA);
	earlymem_add_used((uintptr_t) & __BSS_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __BSS_END -
					    (uintptr_t) & __BSS_START),
			  EARLYMEM_DATA);
	earlymem_add_used((uintptr_t) & __IDMAP_DATA_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __IDMAP_DATA_END -
					    (uintptr_t) & __IDMAP_DATA_START),
			  EARLYMEM_DATA);

	earlymem_add_used((uintptr_t) h, FDT_MAX_SIZE, EARLYMEM_RO);

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
			      MMU_GRANULE_ALIGN((u64) end - (u64) start),
			      EARLYMEM_RO);
	if (r < 0)
		return -1;

	initrd = start;
	initrd_size = (uintptr_t) end - (uintptr_t) start;
	initrd_present = 1;

 einitrd:
	if (!_plat_get_str_prop(h, offset, "bootargs", &cmdline))
		cmdline_present = 1;

	return 0;
}

/**
	Allocator function for allocating page tables before the main kernel
	allocator is available.

	Attempts to map the region of memory in the initial page tables
*/
int _plat_init_alloc(pg_table **p)
{
	int r;

	r = earlymem_alloc(MMU_BLK_SIZE, MMU_BLK_SIZE, (void **)p);
	if (r < 0)
		return r;

	r = mmu_early_map_range(&__idmap_blocks, (void *)*p, MMU_BLK_SIZE,
				MMU_MAP_KERNEL_DATA);

	memset(*p, 0, MMU_BLK_SIZE);

	return r;
}

/**
	plat_init_logical

	Initializes the higher half mapping of the kernel making 0xffff addresses usable

*/
int plat_init_logical()
{
	int r, flags;
	pg_table *pgd;
	uintptr_t base;
	size_t size, len, idx = 0;

	r = _plat_init_alloc(&pgd);
	if (r < 0)
		return r;

	printk("=== Start mapping available memory ===\n");
	earlymem_source_len(&len);
	while (idx < len) {
		r = earlymem_source_ent(idx, &base, &size, &flags);
		if (r < 0)
			return r;
		printk("Mapping region: %#0lx %lx\n", base, size);

		r = mmu_map_range(pgd, (void *)base, size, MMU_MAP_KERNEL_DATA,
				  _plat_init_alloc);

		if (r < 0)
			return r;

		idx++;
	}

	printk("=== Done mapping available memory ===\n");

	mmu_dump(pgd);

	/* Cant use earlymem entries here, as the allocator will mutate them */
	r = mmu_map_range(pgd,
			  (void *)&__TEXT_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __TEXT_END -
					    (uintptr_t) & __TEXT_START),
			  MMU_MAP_KERNEL_CODE, _plat_init_alloc);

	if (r < 0)
		return r;

	r = mmu_map_range(pgd,
			  (void *)&__RODATA_START,
			  MMU_GRANULE_ALIGN((uintptr_t) & __RODATA_END -
					    (uintptr_t) & __RODATA_START),
			  MMU_MAP_KERNEL_RO, _plat_init_alloc);

	if (r < 0)
		return r;

	mmu_dump(pgd);

	asm volatile ("msr ttbr1_el1, %0; isb"::"r" ((uintptr_t) pgd)
	    );

	ktable = pgd;
	return 0;
}

/**
	Writes the address of the kernel commandline supplied by
	the devicetree.

	Returns 0, if the commandline is present and <0 otherwise
*/
int plat_get_cmdline(char **d)
{
	if (!cmdline_present)
		return -1;

	*d = cmdline;
	return 0;
}

void plat_get_ktable(pg_table **d)
{
	*d = ktable;
}

int plat_get_initrd(void **d, size_t *l)
{
	if (!initrd_present)
		return -1;

	*d = initrd;
	*l = initrd_size;
	return 0;
}

/**
    Initialises the output stream of the kernel so that
    printk will start printing things, this should really
    recieve some direction from the bootargs
*/
int plat_init_io()
{
	int r;
	pg_table *k;
	void *uart_addr = (void *)0x09000000;

	plat_get_ktable(&k);

	r = mmu_map_range(k, uart_addr, 0x1000, MMU_MAP_DEVICE,
			  _plat_init_alloc);
	if (r < 0)
		return r;

	pl011_init(va(uart_addr));
	register_console(pl011_puts);
	return 0;
}

int plat_init_mods()
{
	int r;
	char *cmdline;
	struct tar_header *ramdisk;
	size_t ramdisk_len;

	printk("Discovering modules from ramdisk\n");

	r = plat_get_cmdline(&cmdline);
	if (r < 0)
		return r;

	r = plat_get_initrd((void **)&ramdisk, &ramdisk_len);
	if (r < 0)
		return r;

	/* Discovered as a physical address */
	ramdisk = va(ramdisk);
	cmdline = va(cmdline);

	printk("cmdline is: '%s'\n", cmdline);

	r = tar_check_header(ramdisk);
	if (r < 0) {
		return r;
	}

	return 0;
}
