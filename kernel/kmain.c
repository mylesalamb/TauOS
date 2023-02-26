#include <types.h>
#include <aux.h>
#include <gpio.h>
#include <irq.h>
#include <klog.h>
#include <mb.h>
#include <dma.h>
#include <drv/ring.h>
#include <drv/muart.h>
#include <drv/fb.h>
#include <drv/sd.h>
#include <drv/fs/mbr.h>
#include <lib/io.h>
#include <gic.h>
#include <timer.h>
#include <mm/mm.h>
#include <mm/mmu.h>
#include <mm/alloc.h>
#include <dtb.h>

//#define PHYS_BASE_ADDR 0xFE000000
#define PHYS_BASE_ADDR    (0xFE000000 | 0xffff000000000000)
#define DEVICE_MMIO_BEGIN 0x0FC000000
#define DEVICE_MMIO_END   0x100000000

void kmain();
void kinit(void *dtb)
{
        /* 
         * Kernel initialisation tasks
         * Ideally these would be called from
         * a linker-generated array
         */

        

        klog_init(&ring_console);
        mm_init();
        mm_map_peripherals();

        aux_init(PHYS_BASE_ADDR);
        gpio_init(PHYS_BASE_ADDR);
        mb_init(PHYS_BASE_ADDR);
        muart_init();
        klog_init(&muart_console);
        ring_echo(&muart_console);

       

        dma_init(PHYS_BASE_ADDR);
        timer_init(PHYS_BASE_ADDR);

        /* Setup some output devices*/
        io_init(&muart_console);
        muart_init();
        klog_init(&muart_console);

        fb_init();
        
        io_init(&fb_console);
        /* Setup interrupts for the bootcore */
        irq_init(&gic_interface, PHYS_BASE_ADDR);
        gic_irq_enable(0);
        irq_enable();

        printk(IO_GREEN "Kernel initialisation tasks done!\n" IO_RESET);
        printk(IO_GREEN "=================================\n\n" IO_RESET);
        printk("Firmware revision: %h\n", mb_get_firmware_revision());
        printk("Board revision: %h\n", mb_get_board_revision());
        printk("Device tree address: %h\n\n", dtb);
        dtb_init((struct dtb_header *)mm_ptl(dtb));
        kmain();
}

__attribute__((aligned(4))) struct mbr_header mbr;
struct kmem_cache mcache;


void kmain()
{
        printk("Booted to TauOS kernel!\n\n");
        printk("Initialise EMMC2\n");
        sd_init(PHYS_BASE_ADDR);
        sd_seek(0);
        sd_read((u8 *)&mbr, 512);
        printk("\n\n");
        mbr_dump(&mbr);

        printk("Test SLAB Allocator!\n");
        kmemcache_init(&mcache, 128);
        void * ret = ckmalloc(&mcache);
        printk("Returned ptr %h\n", ret);
        ret = ckmalloc(&mcache);
        printk("Returned ptr %h\n", ret);



}
