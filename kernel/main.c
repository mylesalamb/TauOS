#include <tau.h>
#include <pl011.h>
#include <printk.h>
#include <lib/fdt.h>
#include <platform.h>

void __attribute__((noreturn))panic(const char *msg)
{
	/* No error reporting yet so just hang in a known place */
	printk("[panic]: %s\n", msg);
	while (1)
		asm volatile ("nop");
}

void __attribute__((noreturn))kinit(struct fdt_header *dtb)
{

	int r;
	pl011_init(0x09000000);
	register_console(pl011_puts);
	printk("Booting TauOSv%s commit:%s\n", KERNEL_VERSION, GIT_COMMIT_HASH);

	r = platform_early_scan(dtb);
	if (r < 0) {
		panic("bootloader passed corrupted device tree!");
	}

	panic("kinit finished");
}
