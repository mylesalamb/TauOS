#include <types.h>
#include <dma.h>

#define DMA_REGS_OFFSET 0x7000

struct dma_regs *dma_dev;

void dma_init(u64 base_addr)
{
        dma_dev = (struct dma_regs *)(base_addr + DMA_REGS_OFFSET);

}
