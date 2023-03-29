#include <types.h>
#include <drv/blk.h>
#include <lib/io.h>

#define FSINFO_LEAD  0x41615252
#define FSINFO_MID   0x61417272
#define FSINFO_TRAIL 0xAA550000

#define FATTR_RO 0x01
#define FATTR_HIDDEN 0x02
#define FATTR_SYSTEM 0x04
#define FATTR_VOLUMEID 0x08
#define FATTR_DIRECTORY 0x10
#define FATTR_ARCHIVE 0x20
#define FATTR_LFN (FATTR_RO | FATTR_HIDDEN | FATTR_SYSTEM | FATTR_VOLUMEID | FATTR_DIRECTORY | FATTR_ARCHIVE )

struct __attribute__((packed)) fat32_direntry {
        u8 sfilename[11];
        u8 attrs;
        u8 _reserved;
        u8 ctime_s;
        u16 ctime;
        u16 cdate;
        u16 adate;
        u16 cluster_number_high;
        u16 mtime;
        u16 mdate;
        u16 cluster_number_low;
        u32 file_size;
};

struct __attribute__((packed)) fat32_lfn {
        u8 order;
        u8 fname[10];
        u8 attrs;
        u8 long_entry_type;
        u8 checksum;
        u8 fname2[12];
        u16 zero;
        u8 fname3[4];
};

struct __attribute__((packed)) fat32_fsinfo {
        u32 lead_signature;
        u8 _reserved[480];
        u32 mid_signature;
        u32 free_cluster_count;
        u32 free_cluster_number;
        u8 _reserved2[12];
        u32 trail_signature;
};

struct __attribute__((packed)) fat32_bpb {
        /* bios parameter block */
        u8 _jmp_stub[3];
        u8 oem_id[8];
        u16 bytes_per_sector;
        u8 sectors_per_cluster;
        u16 reserved_sectors;
        u8 fat_count;
        u16 root_dir_entries;
        u16 sector_count;
        u8 media_descriptor;
        u16 sectors_per_fat;
        u16 sectors_per_track;
        u16 number_of_heads;
        u32 hidden_sector_count;
        u32 sector_count32;

        /* fat32 extended boot record  */
        u32 sectors_per_fat32;
        u16 media_desccriptor16;
        u16 fat_version;
        u32 root_directory_cluster;
        u16 fs_info_sector;
        u16 fat_boot_sector;
        u8 _reserved[12];
};

u64 _fat_total_sectors(struct fat32_bpb *hdr)
{
        u64 sector_count = 0;
        if(sector_count = bconcat(&hdr->sector_count, sizeof(hdr->sector_count)))
                return sector_count;

        sector_count = bconcat(&hdr->sector_count32, sizeof(hdr->sector_count32));
        return sector_count;
}

u64 _fat_sectors_per_fat(struct fat32_bpb *hdr)
{
        u64 sectors_per_fat = 0;
        if(sectors_per_fat = bconcat(&hdr->sectors_per_fat, sizeof(hdr->sectors_per_fat)))
                return sectors_per_fat;

        sectors_per_fat = bconcat(&hdr->sectors_per_fat32, sizeof(hdr->sectors_per_fat32));
        return sectors_per_fat;
}

u64 _fat_data_sector_offset(struct fat32_bpb *hdr)
{
        u64 reserved_sectors = bconcat(&hdr->reserved_sectors, sizeof(hdr->reserved_sectors));
        u64 fat_size = hdr->fat_count * _fat_sectors_per_fat(hdr);
        return reserved_sectors + fat_size;
}

void _fat_dump_direntry(struct fat32_direntry *dir)
{
        while(dir->sfilename[0])
        {
                if(dir->sfilename[0] == 0xE5 || dir->attrs == 0xf)
                {
                        dir++;
                        continue;
                }
                for(int i = 0; i < sizeof(dir->sfilename); i++)
                {
                        printk("%c", dir->sfilename[i]);
                }
                printk("\n");
                dir++;
        }
}

void fat_init(struct blk_dev *dev, u64 lba)
{
        struct fat32_bpb *hdr = kmalloc(512);
        dev->seek(lba);
        dev->read(hdr, 512);
        printk("Initialise fat filesystem\n");
        printk("-------------------------\n\n");
        printk("jump stub is %h\n", bconcat(&hdr->_jmp_stub), 3);
        printk("oem id is %h\n", bconcat(&hdr->oem_id, sizeof(hdr->oem_id)));
        printk("fat count %h\n", hdr->fat_count);
        printk("start lba %h\n", bconcat(&hdr->hidden_sector_count, sizeof(hdr->hidden_sector_count)));
        printk("Root dir cluster %h\n", bconcat(&hdr->root_directory_cluster), sizeof(hdr->root_directory_cluster));
        printk("bytes per sector: %h\n", bconcat(&hdr->bytes_per_sector, sizeof(hdr->bytes_per_sector)));
        printk("sectors per cluster: %h\n", bconcat(&hdr->sectors_per_cluster, sizeof(hdr->sectors_per_cluster)));
        printk("fsinfo sector: %h\n", bconcat(&hdr->fs_info_sector, sizeof(hdr->fs_info_sector)));
        printk("Reserved sectors: %h\n", bconcat(&hdr->reserved_sectors, sizeof(hdr->reserved_sectors)) );
        printk("Sectors per fat: %h\n", _fat_sectors_per_fat(hdr));

        struct fat32_direntry *dir = kmalloc(512);
        dev->seek(lba + (_fat_data_sector_offset(hdr)));
        dev->read(dir, 512);
        _fat_dump_direntry(dir);
}
