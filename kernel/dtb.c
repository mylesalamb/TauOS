#include <types.h>
#include <klog.h>
#include <dtb.h>
#include <lib/string.h>
#include <lib/common.h>
#include <lib/io.h>


void dtb_dump_reserved(struct dtb_header *header)
{
    struct dtb_mem_reserve *csr;
    u32 offset = btlhw(header->memory_offset);
    
    csr = (struct dtb_mem_reserve *)(((u8 *)header) + offset);

    while(csr->address || csr->size)
    {
        u64 address = btlw(csr->address);
        u64 len = btlw(csr->size);
        // printk("Reserve %h - %h\n", address, address + len);
        csr++;
    }
}

void dtb_dump_struct(struct dtb_header *header)
{
        u32 string_offset = btlhw(header->string_offset);
        u32 struct_offset = btlhw(header->struct_offset);
        
        char *node_name;
        char *prop_name;
        struct dtb_prop *prop = NULL;

        u32 * csr = (u32 *)(((u8 *)header) + struct_offset);
        while(btlhw(*csr) != DTB_END)
        {
                switch (btlhw(*csr))
                {
                        case DTB_BEGIN_NODE:
                                node_name = csr + 1;
                                klog_debug("Entered new node: %s\n", node_name);
                                u64 next = strlen(node_name) + 1;
                                u64 offset = (((u64)node_name + next) + (4 - 1)) & -4;
                                klog_debug("node length %d, skipped from %h to %h\n", next, csr, offset);
                                csr = (u32 *)offset;
                                break;

                        case DTB_END_NODE:
                                node_name = NULL;
                                csr++;
                                break;

                        case DTB_PROP:
                                prop = (struct dtb_prop *)(csr + 1);
                                u64 length = btlhw(prop->length);
                                u64 pnext = (u64)(csr + 1) + sizeof(struct dtb_prop) + length;
                                u64 poffset = (pnext + (4 - 1) )& -4;
                                csr = (u32 *)poffset;
                                break;

                        case DTB_NOP:
                                klog_debug("NOP token, skipping it!\n");
                                csr++;
                                break;
                        default:
                                klog_debug("unrecognised token: %d\n", *csr);
                                csr++;

                }
        
        }
}

void dtb_init(struct dtb_header *header)
{
    //printk("DTB address is: %h\n", header);
    //printk("magic: %h\n", btlhw(header->magic));
    //printk("size: %h\n", btlhw(header->total_size));
    //printk("struct offset: %h\n", btlhw(header->struct_offset));
    //printk("string offset: %h\n", btlhw(header->string_offset));
    //printk("memory offset: %h\n", btlhw(header->memory_offset));
    //printk("version: %h\n", btlhw(header->version));
    //printk("version compat: %h\n", btlhw(header->version_compat));
    //printk("boot cpuid: %h\n", btlhw(header->boot_cpuid));
    //printk("string size: %h\n", btlhw(header->string_size));
    //printk("struct size: %h\n", btlhw(header->struct_size));

    dtb_dump_struct(header);
    dtb_dump_reserved(header);    
}
