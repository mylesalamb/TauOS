#ifndef IO_H
#define IO_H 1

#include <stdarg.h>
#include <types.h>

struct console 
{
        void (*writes)(char *);
};

#define IO_BLACK        "\033[0;30m"
#define IO_RED          "\033[0;31m"
#define IO_GREEN        "\033[0;32m"
#define IO_YELLOW       "\033[0;33m"
#define IO_BLUE         "\033[0;34m"
#define IO_PURPLE       "\033[0;35m"
#define IO_CYAN         "\033[0;36m"
#define IO_WHITE        "\033[0;37m"
#define IO_B_BLACK        "\033[0;40m"
#define IO_B_RED          "\033[0;41m"
#define IO_B_GREEN        "\033[0;42m"
#define IO_B_YELLOW       "\033[0;43m"
#define IO_B_BLUE         "\033[0;44m"
#define IO_B_PURPLE       "\033[0;45m"
#define IO_B_CYAN         "\033[0;46m"
#define IO_B_WHITE        "\033[0;47m"
#define IO_RESET          "\033[0m"

/* Bold colours */
#define IO_BD_BLACK        "\033[1;30m"
#define IO_BD_RED          "\033[1;31m"
#define IO_BD_GREEN        "\033[1;32m"
#define IO_BD_YELLOW       "\033[1;33m"
#define IO_BD_BLUE         "\033[1;34m"
#define IO_BD_PURPLE       "\033[1;35m"
#define IO_BD_CYAN         "\033[1;36m"
#define IO_BD_WHITE        "\033[1;37m"
#define IO_BD_B_BLACK        "\033[1;40m"
#define IO_BD_B_RED          "\033[1;41m"
#define IO_BD_B_GREEN        "\033[1;42m"
#define IO_BD_B_YELLOW       "\033[1;43m"
#define IO_BD_B_BLUE         "\033[1;44m"
#define IO_BD_B_PURPLE       "\033[1;45m"
#define IO_BD_B_CYAN         "\033[1;46m"
#define IO_BD_B_WHITE        "\033[1;47m"

void io_init(struct console *);
u64 cprintk(struct console *, const char *, ...);
u64 vcprintk(struct console *c, const char *fmt, va_list va);
u64 printk(const char *, ...);
u64 sprintf(char *, const char *, ...);

#endif
