#include <stdarg.h>
#include <types.h>
#include <stddef.h>
#include <drv/muart.h>
#include <lib/io.h>
#include <lib/common.h>


void (*_io_dev)(char) = muart_writec;

void io_init(void (*dev)(char))
{
        _io_dev = dev;
}

void io_ptout(void *ptr)
{

}

void io_dtout(void (*dev)(char), i64 num)
{
        u8 nve = 0;
        char *decnums = "0123456789";
        char str[23] = {'\0'};
        u64 last = ARRAY_SZ(str) - 2;
        if(num < 0){
                nve = 1;
                num *= -1;
        }
        for(int i = ARRAY_SZ(str) - 2; i-- > 0; )
        {
                u8 digit = num % 10;
                num /= 10;
                str[i] = decnums[digit];
                if(digit)
                        last = i;
        }
        muart_writes(str + last);
}

void io_utout(void (*dev)(char), u64 num)
{
        char *decnums = "0123456789";
        char str[22] = {'\0'};

        u64 last = ARRAY_SZ(str) - 3;
        for(int i = ARRAY_SZ(str) - 2; i-- > 0; )
        {
                u8 digit = num % 10;
                num /= 10;
                str[i] = decnums[digit];
                if(digit)
                        last = i;
        }
        muart_writes(str + last);
}

void io_htout(void (*dev)(char), u64 num)
{
        char *hexnums = "0123456789ABCDEF";
        /* u64 == 16 hex digits + header + null */
        char str[19] = {'0', 'x'};
        for(int i = (sizeof(str)/sizeof(char)) - 1; i > 1; i-- )
        {
                u8 digit = num & 0xf;
                num >>= 4;
                str[i] = hexnums[digit];
        }
        muart_writes(str);
}

void io_stout(void (*dev)(char), char *str)
{
        muart_writes(str);
}

u64 vcprintk(void (*dev)(char), const char *fmt, va_list va)
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
                                        io_stout(dev, va_arg(va, char*));
                                        break;
                                case 'p':
                                        fmt++;
                                        io_htout(dev, (u64)va_arg(va, void *));
                                        break;
                                case 'd':
                                        fmt++;
                                        i64 n = va_arg(va, i64);
                                        break;
                                case 'u':
                                        fmt++;
                                        io_utout(dev, va_arg(va, u64));
                                        break;
                                case 'c':
                                        fmt++;
                                        char c = va_arg(va, u64);
                                        dev(c);
                                        break;
                                case 'h':
                                        fmt++;
                                        io_htout(dev, va_arg(va, u64));
                                        break;
                                case '%':
                                        dev(*fmt);
                                        fmt++;
                                        break;
                        }
                }
                else
                {
                        muart_writec(*fmt);
                        fmt++;
                }
        }
        return 0;
}

u64 cprintk(void (*dev)(char), const char *fmt, ...)
{
        u64 retval;
        va_list va;
        va_start(va, fmt);
        retval = vcprintk(dev, fmt, va);
        va_end(va);
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
