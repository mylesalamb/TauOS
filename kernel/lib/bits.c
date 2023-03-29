#include <types.h>

/* Given an unaligned integer, and its size*/
/* concatenate the bytes into an aligned int */
u64 bconcat(void *addr, u8 s)
{
        u8 *bytes = addr;
        u64 retval = 0;

        if(!s)
                return retval;

        do
        {
                retval <<= 8;
                retval |= *(bytes + --s);
        }while(s);

        return retval;
}
