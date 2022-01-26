#include <types.h>
#include <gic.h>
#define GIC_OFFSET  0x1840000
#define GICD_OFFSET (GIC_OFFSET + 0x1000)
#define GICC_OFFSET (GIC_OFFSET + 0x2000)

static struct gic_regs gic_dev;

void gic_init(u64 base_addr)
{
        gic_dev.gicc = (struct gicc_regs *)(base_addr + GICC_OFFSET);
        gic_dev.gicd = (struct gicd_regs *)(base_addr + GICD_OFFSET);
}

struct gic_regs *gic_get()
{
        return gic_dev;
}
