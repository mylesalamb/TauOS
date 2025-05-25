#include <tau.h>
#include <pl011.h>
#include <printk.h>
#include <lib/fdt.h>
#include <platform.h>
#include <mm/mmu.h>

extern pg_table __idmap_blocks;
extern pg_table __idmap_blocks_end;
extern char __START, __END;

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
	printk("Kernel occupies [%x,%x]\n", (uintptr_t) & __START,
	       (uintptr_t) & __END);
	printk("idmap occupies [%x,%x]\n", (uintptr_t) & __idmap_blocks,
	       (uintptr_t) & __idmap_blocks_end);
	r = platform_early_scan(dtb);
	if (r < 0) {
		panic("bootloader passed corrupted device tree!");
	}

	mmu_dump(&__idmap_blocks);

	panic("kinit finished");
}
