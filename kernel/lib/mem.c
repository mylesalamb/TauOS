#include <lib/mem.h>

/* gcc will emit calls to these functions, best to define them somewhere */
void memcpy(const void *restrict src, void *restrict dst, size_t n)
{
        
        const u8 *restrict _src = src;
        u8 *restrict _dst = dst;

        while(n--)
        {
                *_dst = *_src;
                ++_src;
                ++_dst;
        }

}

void memset(void *dst, u64 c, size_t n)
{
        u8 *_dst = dst;
        u8 _c = (u8)c;
        while(n--)
        {
                *_dst = _c;
        }

}

void memmove(void *src, void *dst, size_t n)
{
        u8 *_src = src;
        u8 *_dst = dst;

        while(n--)
        {
                *_dst = *_src;
                ++_src;
                ++_dst;
        }
}
