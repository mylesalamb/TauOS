#ifndef FB_H
#define FB_H 1

#include <types.h>
#include <lib/io.h>

enum fb_pallete {
        FB_BLACK     = 0x00,
        FB_RED       = 0x01,
        FB_GREEN     = 0x02,
        FB_YELLOW    = 0x03,
        FB_BLUE      = 0x04,
        FB_MAGENTA   = 0x05,
        FB_CYAN      = 0x06,
        FB_WHITE     = 0x07,
        FB_B_BLACK   = 0x08,
        FB_B_RED     = 0x09,
        FB_B_GREEN   = 0x0a,
        FB_B_YELLOW  = 0x0b,
        FB_B_BLUE    = 0x0c,
        FB_B_MAGENTA = 0x0d,
        FB_B_CYAN    = 0x0e,
        FB_B_WHITE   = 0x0f,
};

u64 _fb_fast_tile_byte(u8);

extern struct console fb_console;

void fb_init();
void fb_writec(char c);
void fb_writes(char *s);
void fb_dma_test();

#endif 
