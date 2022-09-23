#include <types.h>
#include <timer.h>
#include <irq.h>
#include <lib/io.h>

#define TIMER_REGS_OFFSET 0x3000

#define CLOCKHZ 1000000
#define CLOCK_INT (CLOCKHZ / 4)
#define TIMER_IRQ_NUM 97

struct timer_regs *timer_dev;

/*
 * BCMxxxx system timer driver
 *
 * note, thanks to rhythm16 for mentioning that channels 0 and 2 are used by
 * the vc. The interrupt lines associated with these channels appear to sit
 * high, causing the interrupt controller to constantly assert invalid
 * interrupt IDs
 */

void timer_irq_handler();

void timer_init(u64 base_addr)
{
        u32 timer_low;
        timer_dev = (struct timer_regs*)(base_addr + TIMER_REGS_OFFSET);
        

        timer_low = timer_dev->CLO;
        timer_low += CLOCK_INT;
        
        // Clear out the match assuming that one has happened
        timer_dev->CS |= 0x00000010;
        timer_dev->C1 = timer_low;
        irq_install_handler(TIMER_IRQ_NUM, timer_irq_handler);
}

void timer_irq_handler()
{
        printk("Timer tick!\n");
        timer_dev->C1 += CLOCK_INT;
        timer_dev->CS |= 0x00000010;
}
