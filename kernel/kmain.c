#include <types.h>
#include <aux.h>
#include <gpio.h>
#include <irq.h>
#include <mb.h>
#include <drv/muart.h>
#include <drv/fb.h>
#include <lib/io.h>

// As visible to the arm
#define PHYS_BASE_ADDR 0xFE000000
void kmain();
void kinit(void *dtb)
{
        /* 
         * Kernel initialisation tasks
         * Ideally these would be called from
         * a linker-generated array
         */
        aux_init(PHYS_BASE_ADDR);
        gpio_init(PHYS_BASE_ADDR);
        mb_init(PHYS_BASE_ADDR);
        /* io_init needs to come first here */
        io_init(muart_writec);
        muart_init();
        fb_init();
        printk("Kernel initialisation tasks done!\n");
        kmain();
}

void kmain()
{
        printk("Booted to TauOS kernel!\n\n");
        irq_init_vectors();
        printk("Installed IDT\n");
        while(1)
                ;
}
