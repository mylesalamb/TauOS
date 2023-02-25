#ifndef PMM_H
#define PMM_H 1

void pmm_init(u64, u64, void *);

/* Return physical address of a free page */
void * palloc();
/* Free a page by physical address marking it as unused */
void pfree(void *);

/* Mark a range of addresses as used */
void pmarkrange(void *, void *);

#endif
