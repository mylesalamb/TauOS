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

/**
 * @brief Given a string that contains a hex string return
 *        the integer representation of that number
 * 
 * @param src The hex string
 * @return u64 the integer containing the hex string
 */
u64 hstol(char *src)
{
        u64 retval = 0;

        while(*src)
        {
                retval <<= 4;
                if( *src >= 'a' && *src <= 'f' )
                {
                        retval += (*src - 'a') + 10;
                }

                if(*src >= 'A' && *src <= 'F' )
                {
                        retval += (*src - 'A') + 10;
                }

                if(*src >= '0' && *src <= '9' )
                {
                        retval += (*src - '0');
                }


                src++;
        }

        return retval;
}

char *strend(char *src)
{
        while(*src)
                src++;

        return ++src;
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
