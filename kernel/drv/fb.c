#include <mb.h>
#include <drv/font.h>
#include <drv/fb.h>
#include <lib/common.h>
#include <lib/io.h>
#include <lib/mem.h>

struct fb_state
{
        ureg8 *fb_addr;
        u32 fb_size;
        u32 depth;
        u32 pitch;
        u32 offset[2];
} state;

const u32 color_pallete[] = {
       [FB_BLACK]   = 0x00000000,
       [FB_WHITE]   = 0x00ffffff,
       [FB_RED]     = 0x00ff0000,
       [FB_GREEN]   = 0x0000ff00,
       [FB_YELLOW]  = 0x00ffff00,
       [FB_BLUE]    = 0x000000ff,
       [FB_MAGENTA] = 0x00ff00ff,
       [FB_CYAN]    = 0x0000ffff
};
void fb_writes(char *);

struct console fb_console = {
        .writes = fb_writes
};

void fb_init()
{
        struct mbox_hdr *hdr;
        u32 *csr;
        ureg32 *buff = mb_get_buff();  
        hdr = mb_fmt_hdr(buff, 0);
        
        u32 fb_align = 4;
        csr = mb_append_tag((u32 *)hdr->tags, MBOX_FB_ALLOC, 8, 0, &fb_align, sizeof(fb_align));
        
        u32 pres[] = {640,480};
        csr = mb_append_tag(csr, MBOX_SET_PRES, 8, 0, pres, sizeof(pres));
        
        u32 vres[] = {640,480};
        csr = mb_append_tag(csr, MBOX_SET_VRES, 8, 0, vres, sizeof(vres));
        
        u32 depth = 8;
        csr = mb_append_tag(csr, MBOX_SET_DEPTH, 4, 0, &depth, sizeof(depth));
        
        u32 porder = 1;
        csr = mb_append_tag(csr, MBOX_SET_PORDER, 4, 0, &porder, sizeof(porder));
        
        csr = mb_append_tag(csr, MBOX_GET_PITCH, 4, 0, NULL, 0);
       
        u32 palette[ARRAY_SZ(color_pallete) + 2] = {0, ARRAY_SZ(color_pallete)};
        memcpy(color_pallete, palette + 2, sizeof(color_pallete));        
        csr = mb_append_tag(csr, MBOX_SET_PALETTE, sizeof(palette), 0, palette, sizeof(palette));

        mb_finalize_msg(hdr, csr);
        mb_send(hdr, MBOX_ARM_TO_VC);
        
        hdr = mb_recv(MBOX_ARM_TO_VC);
        printk("message size: %h\nmessage code: %h\n", hdr->size, hdr->code);
        
        /* need to add proper traversal of response */
        struct mbox_tag *tag = mb_get_tag(hdr, MBOX_FB_ALLOC);
        if(tag->id == MBOX_FB_ALLOC) {
                printk("got framebuffer tag back \n");
                printk("size is %h\ncode is: %h\n", tag->size, tag->code);
                ureg32 * values = (ureg32 *)tag->value;
                /* should return a phys addr, but of course it doesnt */
                ureg8 *fb_addr = (ureg8 *)(u64)((values[0] | 0x40000000) & ~0xC0000000);
                u32 fb_size = values[1];

                state.fb_addr = fb_addr;
                state.fb_size = fb_size;

                printk("fb_size: %h\nfb_addr: %h\n", fb_size, fb_addr);
        }
        tag = mb_get_tag(hdr, MBOX_GET_PITCH);
        if(tag->id == MBOX_GET_PITCH)
        {
                printk("Got pitch tag back\n");
                
                ureg32 * value = (ureg32 *)tag->value;
                printk("Pitch is: %h\n", *value);
                state.pitch = *value;
        }
        tag = mb_get_tag(hdr, MBOX_SET_PALETTE);
        if(tag->id == MBOX_SET_PALETTE)
        {
                printk("Got set palette code: %h\n", tag->code);
                printk("got value: %h\n", *((ureg32 *)tag->value));
        }
        state.depth = 1;
        state.pitch = 640;
        state.offset[0] = 0;
        state.offset[1] = 0;
}

void fb_writec(char c)
{
        // column
        for(int i = 0; i < 8; i++)
        {
                //row
                for(int j = 0; j < 8; j++)
                {
                        u8 bits = font[(u8)c][j]; 
                        ureg8 *csr = (state.fb_addr + (i + state.offset[0]) + ((640) * (j + state.offset[1])));
                        if(bits & (1 << i))
                        {
                                *csr= FB_WHITE;
                        }
                        else
                        {
                                *csr = FB_BLACK;
                        }
                        
                }
        
        }

        // TODO: replace magic with pres and font size
        state.offset[0] += 8;
        if(state.offset[0] >= state.pitch || c == '\n'){
                state.offset[0] = 0;
                state.offset[1] += 8;
        }

        if(state.offset[1] + 8 >= 480)
        {
                /* Must be memmove as the src and dst are aliased */
                memmove((void *)(state.fb_addr + (state.pitch * 8)), (void *)state.fb_addr, state.fb_size - (state.pitch * 8));
                state.offset[1] -= 8;
        }

}

void fb_writes(char *s)
{
        for(; *s; s++)
                fb_writec(*s);
}
