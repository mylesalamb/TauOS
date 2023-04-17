#ifndef STRING_H
#define STRING_H 1

#include <types.h>

char *strcpy(char *, char *);
u64 strlen(char *);
u8 strcmp(const char *, const char *);
u8 strncmp(const char *, const char *, u64);

#endif
