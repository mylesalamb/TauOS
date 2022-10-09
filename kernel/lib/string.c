#include <types.h>

u64 strlen(char *src)
{
        u64 len = 0;
        while(*src){
                len++;
                src++;
        }

        return len;
}

char *strcpy(char *src, char *dst)
{
        while(*src) {
                *dst = *src;
                dst++;
                src++;
        }
        *dst = '\0';
        return dst;
}
