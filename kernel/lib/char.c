#include <types.h>

u8 isalpha(char c)
{
        return (c <= 90 && c >= 65) || (c <= 122 && c >= 97);
}

u8 isnum(char c)
{
        return c <= 57 && c >= 48;
}

u8 isalnum(char c)
{
        return isalpha(c) || isnum(c);
}
