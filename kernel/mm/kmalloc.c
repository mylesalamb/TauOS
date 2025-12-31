#include <types.h>
#include <printk.h>
#include <mm/kmalloc.h>

int kmalloc_init()
{
    printk("Starting kmalloc\n");
    return 0;
}