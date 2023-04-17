/*************************************************************************
 *     TAUOS IS AN ALTERNATE OPERATING SYSTEM FOR THE RASPBERRYPI 4      *
 *                    COPYRIGHT (C) 2023  MYLES LAMB                     *
 * THIS PROGRAM IS FREE SOFTWARE: YOU CAN REDISTRIBUTE IT AND/OR MODIFY  *
 * IT UNDER THE TERMS OF THE GNU GENERAL PUBLIC LICENSE AS PUBLISHED BY  *
 *   THE FREE SOFTWARE FOUNDATION, EITHER VERSION 3 OF THE LICENSE, OR   *
 *                  (AT YOUR OPTION) ANY LATER VERSION.                  *
 *    THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL,    *
 *    BUT WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF     *
 *     MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  SEE THE     *
 *             GNU GENERAL PUBLIC LICENSE FOR MORE DETAILS.              *
 *   YOU SHOULD HAVE RECEIVED A COPY OF THE GNU GENERAL PUBLIC LICENSE   *
 * ALONG WITH THIS PROGRAM.  IF NOT, SEE <HTTP://WWW.GNU.ORG/LICENSES/>. *
 *************************************************************************/
#include <types.h>
#include <aux.h>
#include <gpio.h>
#include <irq.h>
#include <klog.h>
#include <mb.h>
#include <drv/dma/dma.h>
#include <drv/serial/ring.h>
#include <drv/serial/pl011.h>
#include <drv/serial/muart.h>
#include <drv/video/fb.h>
#include <drv/mmc/sd.h>
#include <drv/common.h>
#include <fs/mbr.h>
#include <lib/io.h>
#include <drv/intc/gic.h>
#include <drv/timer/timer.h>
#include <mm/mm.h>
#include <mm/mmu.h>
#include <mm/alloc.h>
#include <dtb.h>

/* TODO: remove this when we init drivers from device tree */
#define PHYS_BASE_ADDR    (0xFE000000 | 0xffff000000000000)

void qemu_kinit()
{
        pl011_init();
        pl011_writes("Hello world!\n");

        while(1)
                ;
        return;
}

void kmain();

/**
 * @brief kinit is responsible for parsing the flattened device tree
 * passed to the kernel and initiliasing drivers the kernel
 * has been loaded with, and initiliasing the kernel memory
 * subsytem
 * 
 * @param dtbp: the physical address of the device tree,
 *              loaded from firmware or the bootloader etc... 
 */
void kinit(struct dtb_header *dtbp)
{
        /* 
         * Kernel initialisation tasks
         * Ideally these would be called from
         * a linker-generated array
         */
        struct dtb_header * dtbl = mm_ptl(dtbp);
        
        klog_init(&ring_console);
        mm_init(dtbl);
        dtb_init((struct dtb_header *)mm_ptl(dtbl));
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

        kmalloc_init();
        printk(IO_GREEN "Kernel initialisation tasks done!\n" IO_RESET);
        printk(IO_GREEN "=================================\n\n" IO_RESET);
        printk("Firmware revision: %h\n", mb_get_firmware_revision());
        printk("Board revision: %h\n", mb_get_board_revision());
        printk("Device tree address: %h\n\n", dtbl);
        kmain();



        dtb_find_node(dtbl, "memory");
}

__attribute__((aligned(4))) struct mbr_header mbr;

/**
 * @brief kmain is responsible for the hardware agnostic setup of the kernel
 *        and locating the binary of init.elf off of the root disk, and calling
 *        into userspace
 * 
 */
void kmain()
{
        printk("Booted to TauOS kernel!\n\n");
        printk("Initialise EMMC2\n");
        sd_init(PHYS_BASE_ADDR);
        mbr_init(&sd_device);
        struct tauos_driver *drivers = lga_get_array(drivers);
        klog_debug("Loaded lga at %h\n", drivers);
        klog_debug("loaded driver from lga %s\n", drivers->name);

}
