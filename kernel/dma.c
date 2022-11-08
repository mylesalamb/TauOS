#include <types.h>
#include <dma.h>
#include <lib/bitmap.h>

#define DMA_REGS_OFFSET 0x7000
#define DMA_CHANNEL_COUNT 16

#define DMA_LITE_LOW 7
#define DMA_LITE_HI 10
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
               bitmap_clr_bit(&dma_channels, i);
        }

        /* Channel 15 is used by the vpu so mark it as allocated */
        bitmap_set_bit(&dma_channels, 15);

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
                if(!bitmap_is_set(&dma_channels, csr))
                        channel = csr;
        }
        /* Else just try alloc a lite channel */

        bitmap_set_bit(&dma_channels, channel);
        return channel;
}

void dma_start(u8 channel, void *cb)
{
        if(channel > DMA_CHANNEL_COUNT)
                return;

        dma_regs[channel]->conblck_ad = cb;


}

void dma_channel_free(u8 channel)
{
        /* TODO: wait for outstanding transfer */
        bitmap_clr_bit(&dma_channels, channel)
}
