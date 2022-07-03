#ifndef FB_H
#define FB_H 1

#include <lib/io.h>

enum fb_pallete {
        FB_BLACK   = 0x00,
        FB_WHITE   = 0x01,
        FB_RED     = 0x02,
        FB_GREEN   = 0x03,
        FB_YELLOW  = 0x04,
        FB_BLUE    = 0x05,
        FB_MAGENTA = 0x06,
        FB_CYAN    = 0x07,
};

extern struct console fb_console;

void fb_init();
void fb_writec(char c);
void fb_writes(char *s);

#endif 
