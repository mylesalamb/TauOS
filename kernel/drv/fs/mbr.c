#include <klog.h>
#include <lib/io.h>
#include <lib/common.h>
#include <drv/fs/mbr.h>

#define MBR_BOOT_SIGNATURE 0xaa55

#define MBR_HDR_TYPE_NONE  0x00
#define MBR_HDR_TYPE_EXFAT 0x07
#define MBR_HDR_TYPE_FAT32 0x0C



void mbr_dump_part_entry(struct mbr_partition_entry *hdr)
{
        switch(hdr->type)
        {
                case MBR_HDR_TYPE_NONE:
                        printk("Partition type: unallocated\n");
                        break;
                case MBR_HDR_TYPE_EXFAT:
                        printk("Partition type: exfat\n");
                        break; 
                case MBR_HDR_TYPE_FAT32:
                        printk("Partition type: fat32\n");
                        break;
                default:
                        printk("Partition type: unknown (%h)\n", hdr->type);
        } 
}

void mbr_dump(struct mbr_header *hdr)
{
        if(hdr->boot_signature == MBR_BOOT_SIGNATURE)
        {
                printk("Drive signature matches expected: %h\n", hdr->boot_signature);
        }
        else
        {
                printk("Drive signature does not match: %h\n", hdr->boot_signature);
        }
        for(int i = 0; i < ARRAY_SZ(hdr->partition); i++)
        {
                mbr_dump_part_entry(&hdr->partition[i]);
        }

}
