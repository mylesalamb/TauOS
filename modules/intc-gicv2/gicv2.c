#include <kmod.h>
#include <printk.h>

int init_gicv2()
{
    printk("Hello!\n");
    return 0;
}

kmod_init(init_gicv2);
