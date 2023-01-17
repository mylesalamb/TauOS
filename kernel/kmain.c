#include <types.h>
#include <aux.h>
#include <gpio.h>
#include <irq.h>
#include <klog.h>
#include <mb.h>
#include <dma.h>
#include <drv/muart.h>
#include <drv/fb.h>
#include <drv/sd.h>
#include <drv/fs/mbr.h>
#include <lib/io.h>
#include <gic.h>
#include <timer.h>
#include <mm/mmu.h>
#include <mm/pmm.h>

//#define PHYS_BASE_ADDR 0xFE000000
#define PHYS_BASE_ADDR (0x400000 | 0xffff000000000000)
#define DEVICE_MMIO_BEGIN 0x0FC000000
#define DEVICE_MMIO_END   0x100000000
#define PHYS_REAL 0xfffffe0000000000 

void kmain();
void kinit(void *dtb)
{
        /* 
         * Kernel initialisation tasks
         * Ideally these would be called from
         * a linker-generated array
         */

        /* Setup various addresses on the board*/
        aux_init(PHYS_BASE_ADDR);
        gpio_init(PHYS_BASE_ADDR);
        mb_init(PHYS_BASE_ADDR);
        muart_init();
        klog_init(&muart_console);
        mmu_dump_entries();
        pmm_init();
        vmm_init();
        mmu_map_range(PHYS_REAL, DEVICE_MMIO_BEGIN, DEVICE_MMIO_END, MMU_K_DEV_FLAGS);
        mmu_dump_entries();
        while(1)
                ;
        dma_init(PHYS_BASE_ADDR);
        timer_init(PHYS_BASE_ADDR);

        /* Setup some output devices*/
        io_init(&muart_console);
        muart_init();
        klog_init(&muart_console);
        mmu_dump_entries();
        fb_init();
        
        io_init(&fb_console);
        /* Setup interrupts for the bootcore */
        irq_init(&gic_interface, PHYS_BASE_ADDR);
        gic_irq_enable(0);
        irq_enable();

        printk(IO_GREEN "Kernel initialisation tasks done!\n" IO_RESET);
        printk("Firmware revision: %h\n", mb_get_firmware_revision());
        printk("Board revision: %h\n\n", mb_get_board_revision());
        kmain();
}

__attribute__((aligned(4))) struct mbr_header mbr;

void kmain()
{
        printk("Booted to TauOS kernel!\n\n");
        printk("Dumping page tables to uart!\n");

        printk("Initialise EMMC2\n");
        sd_init(PHYS_BASE_ADDR);
        sd_seek(0);
        sd_read(&mbr, 512);
        printk("\n\n");
        mbr_dump(&mbr);

}
