#ifndef MBR_H
#define MBR_H 1

struct mbr_partition_entry{
        u8 status;
        u8 chs_addr_start[3];
        u8 type;
        u8 chs_addr_end[3];
        u32 lba_start;
        u32 lba_len;
};

struct mbr_header{
        u8 bootstrap[446];
        struct mbr_partition_entry partition[4];
        u16 boot_signature;
};

void mbr_dump(struct mbr_header *);
#endif
