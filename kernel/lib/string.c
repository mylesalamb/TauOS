#include <types.h>


u8 strcmp(const char *lhs, const char *rhs)
{
        while(*lhs && *rhs)
        {
                if(*lhs - *rhs)
                        return *lhs - *rhs;

                lhs++;
                rhs++; 
        }
        return *lhs - *rhs;

}

u8 strncmp(const char *lhs, const char *rhs, u64 n)
{
        while(n && *lhs && (*lhs == *rhs))
        {
                ++lhs;
                ++rhs;
                --n;
        }
        if(!n)
        {
                return 0;
        }

        return *lhs - *rhs;


}

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
