#include <tau.h>
#include <pl011.h>
#include <printk.h>
#include <lib/fdt.h>
#include <plat.h>
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

/**
	Early kernel initialisation routine

	Does an initial scan of the device tree
	populating the memory map of the system
	before generating the higher half mapping

	Assumption that all the code here is position independent
	/ will not touch behaviour dependant on .rela
*/
void kinit(struct fdt_header *dtb)
{

	int r;

	/* Need early io to print this, maybe add linux like earlycon */
	printk("Booting TauOSv%s commit:%s\n", KERNEL_VERSION, GIT_COMMIT_HASH);

	/* Work out what memory is available */
	r = plat_early_scan(dtb);
	if (r < 0) {
		panic("plat_early_scan failed!");
	}

	/* Generate logical map for the kernel */
	r = plat_init_logical();
	if (r < 0) {
		panic("plat_init_logical failed!");
	}
}

/* Main kernel entrypoint executed under 0xfff... addresses*/
void kstart(struct fdt_header *dtb)
{
	while (1)
		asm volatile ("nop");
}
