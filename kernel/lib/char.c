#include <types.h>

u8 isalpha(char c)
{
        return (c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a');
}

u8 isnum(char c)
{
        return c <= '9' && c >= '0';
}

u8 isalnum(char c)
{
        return isalpha(c) || isnum(c);
}

u8 ctoi(char c)
{
        if(!isnum(c))
                return 0xff;
        return c - '0';
}
