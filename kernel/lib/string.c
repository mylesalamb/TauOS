#include <stddef.h>

int strcmp(const char *a, const char *b)
{
	while (*a && (*b == *a)) {
		a++;
		b++;
	}

	return (unsigned char)*a - (unsigned char)*b;

}

/**
    strlen - Returns the number of characters contained in @s

    @s: A non null pointer to a string

    returns: The number of characters in the string
*/
size_t strlen(const char *s)
{
	size_t l = 0;

	while (s[l]) {
		l++;
	}

	return l;
}

/**
    strncmp - Compare two strings for equality up to @l characters

    @a: A non null pointer to a string
    @b: A non null pointer to a string
    @l: the maximum number of characters to consider

    returns: >0 iff b comes earlier in a natural sort order
             <0 iff a comes earlier in a natural sort order
             0 if the strings are equal

*/
int strncmp(const char *a, const char *b, size_t l)
{

	while (l && *a && (*b == *a)) {
		a++;
		b++;
		l--;
	}

	if (!l)
		return 0;

	return (unsigned char)*a - (unsigned char)*b;
}

/**
    strchr - Finds the location of the first instance of @chr and returns it

    @a: A non null pointer to a string
    @chr: The character to look for

    returns: The location of the occurrence of the character otherwise
             a pointer to the null byte at the end of the string

*/
const char *strchr(const char *a, int chr)
{
	char c = (char)chr;

	while (*a && *a != c)
		a++;

	return a;
}
