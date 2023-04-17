#ifndef DTB_H
#define DTB_H 1

#include <types.h>

#define DTB_HEADER_MAGIC        0xD00DFEED

#define DTB_BEGIN_NODE          0x00000001
#define DTB_END_NODE            0x00000002
#define DTB_PROP                0x00000003
#define DTB_NOP                 0x00000004
#define DTB_END                 0x00000009

struct dtb_header {
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

struct dtb_node {
        u32 type;
};

struct dtb_prop {
        u32 length;
        u32 name_offset;
};

struct dtb_mem_reserve {
        u64 address;
        u64 size;
};

void dtb_init(struct dtb_header *header);
#endif
