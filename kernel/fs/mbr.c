#include <klog.h>
#include <lib/io.h>
#include <mm/alloc.h>
#include <lib/common.h>
#include <lib/bits.h>
#include <drv/blk.h>
#include <fs/mbr.h>

#define MBR_BOOT_SIGNATURE 0xaa55

#define MBR_HDR_TYPE_NONE  0x00
#define MBR_HDR_TYPE_EXFAT 0x07
#define MBR_HDR_TYPE_LINUX 0X83
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
                case MBR_HDR_TYPE_LINUX:
                        printk("Partition type: linux (EXTx)\n");
                        break;
                default:
                        printk("Partition type: unknown (%h)\n", hdr->type);
        } 
        printk("partition status: %h\n", hdr->status);

        /* These are unligned in the struct so we cast down to u8 * and read out the bytes */
        u32 lba = (u32)bconcat(&hdr->lba_start, sizeof(hdr->lba_start));
        printk("partition logical block address %h\n", lba);

        u32 lba_len = (u32)bconcat(&hdr->lba_len, sizeof(hdr->lba_len));
        printk("partition size in blocks: %h\n\n", lba_len);
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


void mbr_init(struct blk_dev *dev)
{
        struct mbr_header *hdr = kmalloc(sizeof(struct mbr_header));
        
        dev->seek(0);
        dev->read(hdr, sizeof(struct mbr_header));

        if(hdr->boot_signature != MBR_BOOT_SIGNATURE)
        {
                printk("Drive signature does not match: %h\n", hdr->boot_signature);
                printk("Drive does not appear to be MBR partitoned...\n");
                /* TODO: kfree */
                return;
        }

        for(int i = 0; i < ARRAY_SZ(hdr->partition); i++)
        {
                struct mbr_partition_entry *part = &hdr->partition[i];
                /* These are unligned in the struct so we cast down to u8 * and read out the bytes */
                u32 lba = (u32)bconcat(&part->lba_start, sizeof(part->lba_start));
                u32 lba_len = (u32)bconcat(&part->lba_len, sizeof(part->lba_len));


                switch(part->type)
                {
                        case MBR_HDR_TYPE_NONE:
                                klog_debug("Partition %d on disk unallocated\n", i);
                                break; 
                        case MBR_HDR_TYPE_FAT32:
                                klog_debug("Found fat partition, initialise it...\n");
                                fat_init(dev, lba);
                                break;
                        default:
                                klog_debug("No driver for file system with code %h\n", part->type);
                }

        }
}