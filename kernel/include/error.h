#ifndef ERROR_H
#define ERROR_H 1

#define panic(msg) _panic(__FILE__, __LINE__, msg)
void __attribute__((noreturn))_panic(const char *, const int, const char *);
#endif
