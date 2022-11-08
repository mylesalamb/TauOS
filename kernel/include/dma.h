#ifndef DMA_H
#define DMA_H 1

#include <types.h>
#include <lib/common.h>

#define FLAGS_DMA_LITE          BIT(1)
#define FLAGS_DMA_NORM          BIT(2)
#define FLAGS_DMA_DMA4          BIT(3)

/* Various bit-masks for various registers*/
#define CS_RESET                BIT(31)
#define CS_ABORT                BIT(30)
#define CS_DISDEBUG             BIT(29)
#define CS_WAIT_FOR_WRITE       BIT(28)
#define CS_PANIC_PRIO           (BIT(23) | BIT(22) | BIT(21) | BIT(20))
#define CS_PRIO                 (BIT(19) | BIT(18) | BIT(17) | BIT(16))
#define CS_ERROR                BIT(8)
#define CS_WAITING_FOR_WRITE    BIT(8)
#define CS_DREQ_STOPS_DMA       BIT(5)
#define CS_PAUSED               BIT(4)
#define CS_DREQ                 BIT(3)
#define CS_INT                  BIT(2)
#define CS_END                  BIT(1)
#define CS_ACTIVE               BIT(0)

struct dma_cb{
        ureg32 ti;
        ureg32 source_ad;
        ureg32 dest_ad;
        ureg32 transfer_length;
        ureg32 stride;
        ureg32 next;
        ureg32 _reserved[2];
};

struct dma_lite_cb{
        ureg32 ti;
        ureg32 source_ad;
        ureg32 dest_ad;
        ureg32 transfer_length;
        ureg32 _reserved;
        ureg32 next;
        ureg32 _reserved2[2];
};

struct dma4_cb{
        ureg32 ti;
        ureg32 source_ad;
        ureg32 source_info;
        ureg32 dest_ad;
        ureg32 dest_info;
        ureg32 transfer_length;
        ureg32 stride;
        ureg32 next;
        ureg32 _reserved;
};

struct dma_regs {
        ureg32 cs;
        ureg32 conblk_ad;
        ureg32 ti;
        ureg32 source_ad;
        ureg32 dest_ad;
        ureg32 transfer_length;
        ureg32 stride;
        ureg32 next;
        ureg32 dbg;
        ureg32 _reserved[54]
};



#endif
