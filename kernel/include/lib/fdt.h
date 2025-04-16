#ifndef FDT_H
#define FDT_H 1

#include <types.h>
#include <error.h>

#define FDT_HEADER_MAGIC        0xD00DFEED
#define FDT_BEGIN_NODE          0x00000001
#define FDT_END_NODE            0x00000002
#define FDT_NODE_PROP           0x00000003
#define FDT_NODE_NOP            0x00000004
#define FDT_END                 0x00000009

/* Error codes that get returned from fdt functions */
#define FDT_E_BADMAGIC      1
#define FDT_E_BADALIGN      2
#define FDT_E_BADVERSION    3
#define FDT_E_NOTAG         4
#define FDT_E_NOADDR        5
#define FDT_E_NONODE        6
#define FDT_E_NOPROP        7

struct fdt_header {
	u32 magic;
	u32 total_size;
	u32 struct_offset;
	u32 string_offset;
	u32 memory_offset;
	u32 version;
	u32 version_compat;
	u32 boot_cpuid;
	u32 string_size;
	u32 struct_size;
};

struct fdt_token {
	u32 type;
};

struct fdt_prop {
	u32 length;
	u32 name_offset;
};

struct fdt_mem_reserve {
	u64 address;
	u64 size;
};

int fdt_check_header(const struct fdt_header *);

void fdt_traverse(const struct fdt_header *);
int fdt_nodepath_offset(const struct fdt_header *, const char *, uint *);
int fdt_node_depth(const struct fdt_header *, uint);
int fdt_nodeparent(const struct fdt_header *, uint, uint *);
int fdt_getprop(const struct fdt_header *, const uint, const char *,
		void **, u32 *);

int fdt_size_cells(const struct fdt_header *, uint);
int fdt_address_cells(const struct fdt_header *, uint);
int fdt_cell(const struct fdt_header *, uint, const char *);
#endif
