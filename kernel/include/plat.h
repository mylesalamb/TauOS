#ifndef PLAT_H
#define PLAT_H 1

#include <lib/fdt.h>
#include <mm/mmu.h>

int plat_early_scan(const struct fdt_header *);
int plat_init_logical();
int plat_init_io();
int plat_init_mods();

int plat_get_cmdline(char **);
void plat_get_ktable(pg_table **);
int plat_get_initrd(void **, size_t *);

#endif
