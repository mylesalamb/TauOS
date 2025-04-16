#include <stddef.h>
#include <types.h>
#include <lib/fdt.h>
#include <printk.h>
#include <endian.h>

#define MAX_FDT_CELL_SIZE 2

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

		printk("Memory entry %d [%#0lx, %#0lx)\n", i, address,
		       size + 0x40000000);
	}

	return 0;
}

int platform_early_scan_chosen(const struct fdt_header *h)
{
	int retval;
	unsigned int offset = 0;
	const char *chosen = "/chosen";
	retval = fdt_nodepath_offset(h, chosen, &offset);
	if (retval < 0) {
		return retval;
	}

	return 0;
}
