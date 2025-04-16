#include <stddef.h>
#include <types.h>
#include <lib/fdt.h>
#include <printk.h>
#include <endian.h>
#include <mm/earlymem.h>

#define MAX_FDT_CELL_SIZE 2

extern char __TEXT_START;
extern char __TEXT_END;

extern char __RODATA_START;
extern char __RODATA_END;

extern char __DATA_START;
extern char __DATA_END;

extern char __BSS_START;
extern char __BSS_END;

int platform_early_scan_memory(const struct fdt_header *h);
int platform_early_scan_chosen(const struct fdt_header *h);

int platform_early_scan(const struct fdt_header *h)
{
	int r;

	r = fdt_check_header(h);
	if (r < 0) {
		return -1;
	}

	r = platform_early_scan_memory(h);
	if (r < 0) {
		return -1;
	}

	r = platform_early_scan_chosen(h);
	if (r < 0) {
		return -1;
	}

	return 0;
}

/**
    platform_early_scan_memory:


*/
int platform_early_scan_memory(const struct fdt_header *h)
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
			  (uintptr_t) & __TEXT_END -
			  (uintptr_t) & __TEXT_START);
	earlymem_add_used((uintptr_t) & __RODATA_START,
			  (uintptr_t) & __RODATA_END -
			  (uintptr_t) & __RODATA_START);
	earlymem_add_used((uintptr_t) & __DATA_START,
			  (uintptr_t) & __DATA_END -
			  (uintptr_t) & __DATA_START);
	earlymem_add_used((uintptr_t) & __BSS_START,
			  (uintptr_t) & __BSS_END - (uintptr_t) & __BSS_START);

	return 0;
}

int _platform_get_addr_prop(const struct fdt_header *h, unsigned int offset,
			    const char *name, u64 *v)
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
	*v = beth64(b);
	return 0;
}

int platform_early_scan_chosen(const struct fdt_header *h)
{
	int retval;
	u64 start, end;
	unsigned int offset = 0;
	const char *chosen = "/chosen";
	retval = fdt_nodepath_offset(h, chosen, &offset);
	if (retval < 0) {
		return retval;
	}

	retval =
	    _platform_get_addr_prop(h, offset, "linux,initrd-start", &start);
	if (retval < 0)
		return 0;

	retval = _platform_get_addr_prop(h, offset, "linux,initrd-end", &end);
	if (retval < 0)
		return 0;

	earlymem_add_used(start, end - start);

	return 0;
}
