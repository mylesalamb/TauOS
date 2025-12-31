#include <tau.h>
#include <exc.h>
#include <error.h>
#include <printk.h>
#include <plat.h>
#include <lib/fdt.h>

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
void __attribute__((noreturn))kstart(struct fdt_header *dtb)
{
	int r;
	exc_init();

	r = plat_init_io();
	if (r < 0) {
		panic("plat_init_io failed!");
	}
	
	r = plat_init_mm();
	if (r < 0) {
		panic("plat_init_mm failed!");
	}

	r = plat_init_mods();
	if (r < 0) {
		panic("plat_init_mods failed!");
	}

	panic("kstart tried to return");
}
