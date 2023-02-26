#include <types.h>
#include <dtb.h>
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
        printk("Reserve %h - %h\n", address, address + len);
        csr++;
    }
}

void dtb_init(struct dtb_header *header)
{
    printk("DTB address is: %h\n", header);
    printk("magic: %h\n", btlhw(header->magic));
    printk("size: %h\n", btlhw(header->total_size));
    printk("struct offset: %h\n", btlhw(header->struct_offset));
    printk("string offset: %h\n", btlhw(header->string_offset));
    printk("memory offset: %h\n", btlhw(header->memory_offset));
    printk("version: %h\n", btlhw(header->version));
    printk("version compat: %h\n", btlhw(header->version_compat));
    printk("boot cpuid: %h\n", btlhw(header->boot_cpuid));
    printk("string size: %h\n", btlhw(header->string_size));
    printk("struct size: %h\n", btlhw(header->struct_size));

    dtb_dump_reserved(header);    
}
