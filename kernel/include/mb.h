#ifndef MB_H
#define MB_H 1

#include <types.h>

#define PHY_OFFSET_MB_REGS 0xB880

#define MB_MSG_MASK 0x80000000

#define MB_CLOCK_ID_EMMC2 0x00000000c

enum mbox_tags {
        MBOX_FW_VERSION  = 0x00000001,
        MBOX_BOARD_MODEL = 0x00010001,
        MBOX_BOARD_REV   = 0x00010002,
        MBOX_BOARD_MAC   = 0x00010003,
        MBOX_BOARD_SER   = 0x00010004,
        MBOX_ARM_MEM     = 0x00010005,
        MBOX_GET_CLOCK_RATE   = 0x00030002,
        MBOX_SET_CLOCK_RATE   = 0x00038002, 
        MBOX_CLOCKS      = 0x00010007,
        MBOX_CMD_LINE    = 0x00050001,
        MBOX_DMA_CHANNEL = 0x00060001,
        MBOX_GET_POWER   = 0x00020001,
        MBOX_SET_POWER   = 0x00028001,

        MBOX_VC_MEM      = 0x00010006,
        MBOX_FB_ALLOC    = 0x00040001,
        MBOX_FB_FREE     = 0x00048001,
        MBOX_GET_PRES    = 0x00040003,
        MBOX_SET_PRES    = 0x00048003,
        MBOX_SET_VRES    = 0x00048004,
        MBOX_GET_VRES    = 0x00040004,
        MBOX_GET_DEPTH   = 0x00040005, 
        MBOX_SET_DEPTH   = 0x00048005,
        MBOX_GET_PORDER  = 0x00040006,
        MBOX_SET_PORDER  = 0x00048006,
        MBOX_GET_VOFFSET = 0x00040009,
        MBOX_SET_VOFFSET = 0x00048009,
        MBOX_GET_PALETTE = 0x0004000b,
        MBOX_SET_PALETTE = 0x0004800b,
        MBOX_GET_PITCH   = 0x00040008,
};

enum mbox_channel {
        MBOX_FB        = 0x1,
        MBOX_ARM_TO_VC = 0x8,
        MBOX_VC_TO_ARM = 0x9,
};

enum mbox_status {
        MBOX_WFULL  = 0x80000000,
        MBOX_REMPTY = 0x40000000,
};

struct mbox_regs {
        ureg32 read;
        ureg32 _reserved[5];
        ureg32 sender;
        ureg32 status;
        ureg32 config;
        ureg32 write;
};

struct mbox_hdr {
        ureg32 size;
        ureg32 code;
        ureg8 tags[];
};

struct mbox_tag {
        ureg32 id;
        ureg32 size;
        ureg32 code;
        ureg8 value[];
};


struct mbox_regs *mb_init(u64);
ureg32 *mb_get_buff();
struct mbox_hdr *mb_fmt_hdr(ureg32 *, u32);
void mb_finalize_msg(void *, void *);
u32 *mb_append_tag(u32*, u32, u32, u32, void*, u64);
void mb_send(struct mbox_hdr *, u8 );

/* Define some stuff to make things a little more readable elsewhere */
u32 mb_get_board_model();
u32 mb_get_firmware_revision();
u32 mb_get_board_revision();
u32 mb_get_clock_rate(u32);
void mb_set_clock_rate(u32, u32);
struct mbox_hdr *mb_recv(u8);
struct mbox_tag *mb_get_tag(struct mbox_hdr *hdr, u32 code);
#endif
