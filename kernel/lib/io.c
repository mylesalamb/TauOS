#include <stdarg.h>
#include <types.h>
#include <stddef.h>
#include <drv/muart.h>
#include <lib/io.h>
#include <lib/common.h>
#include <lib/string.h>

static struct console *_io_dev;

void io_init(struct console *c)
{
        _io_dev = c;
}

char *itoa(i64 num, char *dst)
{
        u8 nve = 0;
        char *decnums = "0123456789";
        char str[23] = {'\0'};
        u64 last = ARRAY_SZ(str) - 2;
        if(num < 0){
                nve = 1;
                num *= -1;
        }
        for(int i = ARRAY_SZ(str) - 2; i >= 0; i-- )
        {
                u8 digit = num % 10;
                num /= 10;
                str[i] = decnums[digit];
                if(digit)
                        last = i;
        }
        return strcpy(str + last, dst);
}

char *utoa(u64 num, char *dst)
{
        char *decnums = "0123456789";
        char str[22] = {'\0'};

        u64 last = ARRAY_SZ(str) - 2;
        for(int i = ARRAY_SZ(str) - 2; i >= 0; i--)
        {
                u8 digit = num % 10;
                num /= 10;
                str[i] = decnums[digit];
                if(digit)
                        last = i;
        }

        return strcpy(str + last, dst);

}

char *htoa(u64 num, char *dst)
{
        char *hexnums = "0123456789ABCDEF";
        /* u64 == 16 hex digits + header + null */
        char str[19] = {'0', 'x'};
        for(int i = ARRAY_SZ(str) - 2; i > 1; i-- )
        {
                u8 digit = num & 0xf;
                num >>= 4;
                str[i] = hexnums[digit];
        }
        return strcpy(str, dst);
}

char *stos(char *src, char *dst)
{
        /* Basically a memcpy that returns the end of the string */
        return strcpy(src, dst);
}

u64 vsprintf(char *dst, const char *fmt, va_list va)
{

        while(*fmt)
        {
                if(*fmt == '%')
                {
                        fmt++;
                        switch(*fmt)
                        {
                                case 's':
                                        fmt++;
                                        dst = stos(va_arg(va, char*), dst);
                                        break;
                                case 'p':
                                        fmt++;
                                        dst = htoa((u64)va_arg(va, void *), dst);
                                        break;
                                case 'd':
                                        fmt++;
                                        dst = itoa(va_arg(va, i64), dst);
                                        break;
                                case 'u':
                                        fmt++;
                                        dst = utoa(va_arg(va, u64), dst);
                                        break;
                                case 'c':
                                        fmt++;
                                        char c = va_arg(va, u64);
                                        *dst = c;
                                        dst++;
                                        *dst = '\0';
                                        break;
                                case 'h':
                                        fmt++;
                                        dst = htoa(va_arg(va, u64), dst);
                                        break;
                                case '%':
                                        *dst = *fmt;
                                        fmt++;
                                        dst++;
                                        *dst = '\0';
                                        break;
                        }

                }
                else
                {
                        *dst = *fmt;
                        dst++;
                        fmt++;
                }

        }
        return 0;

}

u64 vcprintk(struct console *c, const char *fmt, va_list va)
{
        u64 retval;
        char buff[128] = {0};
        retval = vsprintf(buff, fmt, va);
        c->writes(buff);
        return retval;
}

u64 printk(const char *fmt, ...)
{
        u64 retval;
        va_list va;
        va_start(va, fmt);
        retval = vcprintk(_io_dev, fmt, va);
        va_end(va);
        return retval;
}

u64 cprintk(struct console *c, const char *fmt, ...)
{
        u64 retval;
        va_list va;
        va_start(va, fmt);
        retval = vcprintk(c, fmt, va);
        va_end(va);
        return retval;
}


u64 sprintf(char *dst, const char *fmt, ...)
{
        u64 retval;
        va_list va;
        va_start(va, fmt);
        retval = vsprintf(dst, fmt, va);
        va_end(va);
        return retval;
}

