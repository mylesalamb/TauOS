#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char *s);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);
const char *strchr(const char *, int);

#endif
