#include <mb.h>
#include <klog.h>
#include <drv/video/font.h>
#include <drv/video/fb.h>
#include <mm/mm.h>
#include <lib/common.h>
#include <lib/io.h>
#include <lib/mem.h>
#include <lib/char.h>
#include <drv/dma/dma.h>

struct fb_state
{
        ureg8 *fb_addr;
        u32 fb_size;
        u32 depth;
        u32 pitch;
        u32 offset[2];
        u32 fg;
        u32 bg;
        u8 ansi_esc;
        u8 ansi_count;
        u8 ansi_arg[2];

} state;

static ureg32 cb_buff[256] __attribute__((aligned(32)));

#define FB_DEFAULT_FG FB_WHITE
#define FB_DEFAULT_BG FB_BLACK
#define FB_BOLD_OFFSET FB_B_BLACK

#define ANSI_IS_FG(x) (x >= 30 && x <= 37)
#define ANSI_IS_BG(x) (x >= 40 && x <= 47)

const u32 color_pallete[] = {
        // Standard colours
        [FB_BLACK]   = 0x00000000,
        [FB_RED]     = 0x000000aa,
        [FB_GREEN]   = 0x0000aa00,
        [FB_YELLOW]  = 0x000055aa,
        [FB_BLUE]    = 0x00aa0000,
        [FB_MAGENTA] = 0x00aa00aa,
        [FB_CYAN]    = 0x00aaaa00,
        [FB_WHITE]   = 0x00aaaaaa,
        // Bright colours
        [FB_B_BLACK]   = 0x00aaaaaa,
        [FB_B_RED]     = 0x00aaaaff,
        [FB_B_GREEN]   = 0x00aaffaa,
        [FB_B_YELLOW]  = 0x00aaffff,
        [FB_B_BLUE]    = 0x00ffaaaa,
        [FB_B_MAGENTA] = 0x00ffaaff,
        [FB_B_CYAN]    = 0x00ffffaa,
        [FB_B_WHITE]   = 0x00ffffff,
};
void fb_writes(char *);

struct console fb_console = {
        .writes = fb_writes,
        .writec = fb_writec
};

void fb_init()
{
        struct mbox_hdr *hdr;
        u32 *csr;
        ureg32 *buff = mb_get_buff();  
        hdr = mb_fmt_hdr(buff, 0);

        csr = (u32 *)hdr->tags;

        u32 pres[] = {640,480};
        csr = mb_append_tag(csr, MBOX_SET_PRES, 8, 0, pres, sizeof(pres));

        u32 vres[] = {640,480};
        csr = mb_append_tag(csr, MBOX_SET_VRES, 8, 0, vres, sizeof(vres));

        u32 depth = 8;
        csr = mb_append_tag(csr, MBOX_SET_DEPTH, 4, 0, &depth, sizeof(depth));

        csr = mb_append_tag(csr, MBOX_GET_PITCH, 4, 0, NULL, 0);

        u32 palette[ARRAY_SZ(color_pallete) + 2] = {0, ARRAY_SZ(color_pallete)};
        memcpy(color_pallete, palette + 2, sizeof(color_pallete));        
        csr = mb_append_tag(csr, MBOX_SET_PALETTE, sizeof(palette), 0, palette, sizeof(palette));

        u32 fb_align = 4;
        csr = mb_append_tag(csr, MBOX_FB_ALLOC, 8, 0, &fb_align, sizeof(fb_align));


        mb_finalize_msg(hdr, csr);
        mb_send(hdr, MBOX_ARM_TO_VC);

        hdr = mb_recv(MBOX_ARM_TO_VC);

        if(hdr->code & ~MB_MSG_MASK)
        {
                klog_error("Mailbox call failed! header code was %h\n", hdr->code);
                return;
        }

        /* need to add proper traversal of response */


        struct mbox_tag *tag = mb_get_tag(hdr, MBOX_FB_ALLOC);
        if(tag->id == MBOX_FB_ALLOC) {
                klog_debug("got framebuffer tag back \n");
                klog_debug("size is %h\ncode is: %h\n", tag->size, tag->code);

                ureg32 * values = (ureg32 *)tag->value;

                ureg8 *fb_bus_addr = (ureg8 *)(u64)(values[0]);
                u32 fb_size = values[1];
                state.fb_addr = (ureg8 *)mm_ptl(BUS_TO_PHYS((u64)fb_bus_addr));
                state.fb_size = fb_size;

                klog_debug("Framebuffer address is BUS_ADDR(%h) => PHYS_ADDR(%h)\n", fb_bus_addr, BUS_TO_PHYS((u64)fb_bus_addr));
                klog_debug("Framebuffer size is %h\n", fb_size);
        }


        tag = mb_get_tag(hdr, MBOX_GET_PITCH);
        if(tag->id == MBOX_GET_PITCH)
        {
                klog_debug("Got pitch tag back\n");

                ureg32 * value = (ureg32 *)tag->value;
                klog_debug("Pitch is: %h\n", *value);
                state.pitch = *value;
        }


        tag = mb_get_tag(hdr, MBOX_SET_PALETTE);
        if(tag->id == MBOX_SET_PALETTE)
        {
                klog_debug("Got set palette code: %h\n", tag->code);
                klog_debug("got value: %h\n", *((ureg32 *)tag->value));
        }


        state.depth = 1;
        state.pitch = 640;
        state.offset[0] = 0;
        state.offset[1] = 0;
        state.fg = FB_DEFAULT_FG;
        state.bg = FB_DEFAULT_BG;
}

void fb_set_colour()
{
        if(state.ansi_count == 0 && state.ansi_arg[0] == 0)
        {
                state.bg = FB_DEFAULT_BG;
                state.fg = FB_DEFAULT_FG;
                return;
        }

        /* I only handle the two argument form of [<bold>;<colour_code>m */
        if(state.ansi_count == 1)
        {
                u32 bold = state.ansi_arg[0] ? FB_BOLD_OFFSET : 0 ;
                if(ANSI_IS_FG(state.ansi_arg[1]))
                {
                        state.fg = bold + state.ansi_arg[1] - 30;
                }
                if(ANSI_IS_BG(state.ansi_arg[1]))
                {
                        state.bg = bold + state.ansi_arg[1] - 40;
                }

        }
}

void fb_parse_ansi(char c)
{
        // We are targetting strictly vt100 here
        // Anything is else is too complex / cant be bothered
        if(!state.ansi_esc)
        {
                state.ansi_esc = 1;
                state.ansi_count = 0;
                state.ansi_arg[0] = 0;
                state.ansi_arg[1] = 0;

        }

        if(isalpha(c))
        {
                switch(c)
                {
                        case 'm':
                                fb_set_colour();
                                break;
                        default:
                                break;

                }
                // After reading and executing the command we are done
                state.ansi_esc = 0;
        }
        if(c == '[')
        {
                return;
        }

        if(c == ';')
        {
                state.ansi_count++;
                return;
        }
        if(isnum(c))
        {
                state.ansi_arg[state.ansi_count] = (state.ansi_arg[state.ansi_count] * 10 + ctoi(c));
        }



}

void fb_writec(char c)
{
        if(c == '\033' || state.ansi_esc)
        {
                fb_parse_ansi(c);
                return;
        }

        /* Get a row of a character and write the whole lot out in one write */
        for(int j = 0; j < 8; j++)
        {
                u8 bits = (u64)font[(u8)c][j]; 
                ureg64 *csr = (ureg64 *)(state.fb_addr + (state.offset[0]) + ((640) * (j + state.offset[1])));
                u64 fg_row = _fb_fast_tile_byte(bits);
                u64 bg_row = (~fg_row) & 0x0101010101010101;
                *csr = (fg_row * state.fg) | (bg_row * state.bg);
        }

        // TODO: replace magic with pres and font size
        state.offset[0] += 8;
        if(state.offset[0] >= state.pitch || c == '\n'){
                state.offset[0] = 0;
                state.offset[1] += 8;
        }

        if(state.offset[1] + 8 >= 480)
        {
                u8 channel;
                channel = dma_channel_alloc(FLAGS_DMA_NORM);
                dma_cb_init_memcpy(channel, cb_buff, (void *)(state.fb_addr + (state.pitch * 8)), (void *)state.fb_addr, state.fb_size - (state.pitch * 8));
                dma_start(channel, cb_buff);
                dma_channel_free(channel);
                state.offset[1] -= 8;
        }

}

void fb_writes(char *s)
{
        for(; *s; s++)
                fb_writec(*s);
}


void fb_dma_test()
{
        u8 dma_channel = dma_channel_alloc(FLAGS_DMA_NORM);
        u32 arr[10] = {1}; 
        for(int i = 0; i < ARRAY_SZ(arr); i++)
                arr[i] = 0x01010101;

        dma_cb_init_memcpy(dma_channel, cb_buff, arr, state.fb_addr, 640 * 480);
        klog_debug("starting dma copy\n");
        dma_start(dma_channel, cb_buff);

        while(1)
                ;
}
