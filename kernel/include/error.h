#ifndef ERROR_H
#define ERROR_H 1
/* Macros and types for pretty printing errors that come back from modules */
struct errtabent {
	const char *str;
};

#define ERRTABENT(val) \
    [(val)] = { .str = #val, }
#define ERRTABSIZE(err)	((int)(sizeof(err) / sizeof(err[0])))

#endif
