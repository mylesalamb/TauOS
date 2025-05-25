#ifndef PLAT_H
#define PLAT_H 1

extern char *cmdline;
extern void *initrd;

int plat_early_scan(const struct fdt_header *);
int plat_init_logical();

#endif
