#include <klog.h>
#include <lib/io.h>
#include <lib/common.h>
#include <drv/fs/mbr.h>

#define MBR_BOOT_SIGNATURE 0x55aa

#define MBR_HDR_TYPE_NONE  0x00
#define MBR_HDR_TYPE_EXFAT 0x07
#define MBR_HDR_TYPE_FAT32 0x0C



void mbr_dump_part_entry(struct mbr_partition_entry *hdr)
{
        switch(hdr->type)
        {
                case MBR_HDR_TYPE_NONE:
                        klog_debug("Partition is not allocated!\n");
                case MBR_HDR_TYPE_EXFAT:
                        klog_debug("Partition type: exfat\n");
                        break; 
                case MBR_HDR_TYPE_FAT32:
                        klog_debug("Partition type: fat32\n");
                        break;
                default:
                        klog_debug("Filesystem type not recognised!\n");
        } 
}

void mbr_dump(struct mbr_header *hdr)
{
        if(hdr->boot_signature == MBR_BOOT_SIGNATURE)
        {
                klog_debug("Drive signature matches expected: %h\n", hdr->boot_signature);
        }
        else
        {
                klog_debug("Drive signature does not match: %h\n", hdr->boot_signature);
        }
        for(int i = 0; i < ARRAY_SZ(hdr->partition); i++)
        {
                mbr_dump_part_entry(&hdr->partition[i]);
        }

}
