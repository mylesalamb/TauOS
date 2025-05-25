#include <stddef.h>

void *memset(void *dest, int c, size_t s)
{
	unsigned char f = (unsigned char)c;
	unsigned char *d = (unsigned char *)dest;

	while (s--)
		*d++ = f;

	return dest;
}

void *memmove(void *dest, void *src, size_t n)
{
	unsigned char *d = dest;
	unsigned char *s = src;

	while (n--)
		*d++ = *s++;

	return dest;
}
