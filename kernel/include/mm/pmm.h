#ifndef PMM_H
#define PMM_H 1

void pmm_init();

/* Return physical address of a free page */
void * palloc();
/* Free a page by physical address marking it as unused */
void pfree(void *);

#endif
