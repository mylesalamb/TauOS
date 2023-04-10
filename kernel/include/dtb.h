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
        ureg32 magic;
        ureg32 total_size;
        ureg32 struct_offset;
        ureg32 string_offset;
        ureg32 memory_offset;
        ureg32 version;
        ureg32 version_compat;
        ureg32 boot_cpuid;
        ureg32 string_size;
        ureg32 struct_size;
};

struct dtb_prop {
        ureg32 length;
        ureg32 name_offset;
};

struct dtb_mem_reserve {
        ureg64 address;
        ureg64 size;
};

void dtb_init(struct dtb_header *header);
#endif
