#include <types.h>
#include <drv/dma/dma.h>
#include <klog.h>
#include <mm/mm.h>
#include <drv/common.h>
#include <lib/bitmap.h>
#include <lib/mem.h>

#define DMA_REGS_OFFSET 0x7000
#define DMA_CHANNEL_COUNT 16

#define DMA_LITE_LOW 7
#define DMA_LITE_HIGH 10
#define DMA_IS_LITE(x) ( (x >= 7) && (x <= 10) )

#define DMA_NORM_LOW 0
#define DMA_NORM_HIGH 6
#define DMA_IS_NORM(x) (x <= 6)

#define DMA_DMA4_LOW 10
#define DMA_DMA4_HIGH 15
#define DMA_IS_DMA4(x) ( ( x >= 10) && ( x <= 15))

struct dma_regs *dma_dev;

BITMAP_INIT(dma_channels, 16);

/* 
 * DMA controller has several different types of dma chips attached to it, we should 
 * seek to provide a unified interface for all of them 
 */

void dma_init(u64 base_addr)
{
        dma_dev = (struct dma_regs *)(base_addr + DMA_REGS_OFFSET);

        for(int i = 0; i < DMA_CHANNEL_COUNT; i++)
        {
               bitmap_clr_bit(dma_channels, i);
        }

        /* Channel 15 is used by the vpu so mark it as allocated */
        bitmap_set_bit(dma_channels, 15);

}


/* Given a flag indicating the type, allocate a channel of that type */
u8 dma_channel_alloc(u8 flags)
{
        u8 channel = 0xff;
        u8 low = 0;
        u8 high = 0;
        if(flags & FLAGS_DMA_NORM)
        {
                low = DMA_NORM_LOW;
                high = DMA_NORM_HIGH;
        }
        if(flags & FLAGS_DMA_DMA4)
        {
                low = DMA_DMA4_LOW;
                high = DMA_DMA4_HIGH;
        }
        if(flags & FLAGS_DMA_LITE)
        {
                low = DMA_LITE_LOW;
                high = DMA_LITE_HIGH;
        }

        for(u8 csr = low ; csr <= high; csr++)
        {
                if(!bitmap_is_set(dma_channels, csr)){
                        channel = csr;
                        break;
                }
        }
        klog_debug("Allocate channel %h\n", channel);
        bitmap_set_bit(dma_channels, channel);
        return channel;
}


void dma_cb_init_memcpy(u8 channel, void *cb, void *src, void *dst, size_t n)
{
        if(((u64)cb) & 0x1f)
        {
                klog_error("Cannot use provided control block as it fails alignment check: %h\n", (u64)cb);
                return;
        }

        if(DMA_IS_NORM(channel))
        {
                klog_debug("Setup DMA (norm) memcpy\n");
                memzero(cb, sizeof(struct dma_lite_cb));
                struct dma_lite_cb *lite_cb = cb;
                lite_cb->source_ad = (u32)PHYS_TO_BUS(mm_ltp(src));
                lite_cb->dest_ad = (u32)PHYS_TO_BUS(mm_ltp(dst));
                lite_cb->ti = TI_DESTINC | TI_SRCINC | TI_SRCWIDTH | TI_DESTWIDTH;
                lite_cb->transfer_length = n;

        }
        if(DMA_IS_LITE(channel))
        {
                struct dma_cb *norm_cb = cb;

        }
        if(DMA_IS_DMA4(channel))
        {
                struct dma_dma4_cb *dma4_cb = cb;
        }

}

void dma_start(u8 channel, void *cb)
{
        if(channel > DMA_CHANNEL_COUNT)
                return;
        
        klog_debug("Start DMA transfer on channel %h\n", channel);

        dma_dev[channel].conblk_ad = (u32)PHYS_TO_BUS((u64)cb);
        dma_dev[channel].cs = CS_ACTIVE;
}

void dma_wait(u8 channel)
{
        while(dma_dev[channel].cs & CS_ACTIVE)
                ;
}

void dma_channel_free(u8 channel)
{
        /* TODO: wait for outstanding transfer */
        dma_wait(channel);
        bitmap_clr_bit(dma_channels, channel);
}


struct tauos_device_compat bcm_dma_compat[] = {
        { .compatible = "brcm,bcm2835-dma" },
        { /* Sentinel */}
};

TAU_DRIVER(bcm_dma) = {
       .name = "Broadcom DMA controller driver",
       .dclass = DCLASS_DMA,
       .compat = bcm_dma_compat
};