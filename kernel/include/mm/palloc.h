
#ifndef MM_PALLOC_H
#define MM_PALLOC_H 1

enum {
	PALLOC_4KB = 0,
	PALLOC_8KB,
	PALLOC_16KB,
	PALLOC_32KB,
	PALLOC_64KB,
	PALLOC_128KB,
	PALLOC_256KB,
	PALLOC_512KB,
	PALLOC_1MB,
	PALLOC_2MB,
	PALLOC_4MB,
	_PALLOC_LIMIT,
};

int palloc_init();
int palloc(void **, unsigned int);
int pfree(void *);
#endif
