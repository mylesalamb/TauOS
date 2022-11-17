.global _fb_fast_tile_byte

/* Roughly this code imitates a fn call to*/
/* Its the fastest way I could figure out how to make the framebuffer write */
/* rows at a time. Shouldnt make much difference as the multi-line copy */
/* is where this code spends most of its time */
/*
* 
* u64 fast_tile_byte(u8 bits)
* {
*         u64 magic = 0x2000800200080;
*         u64 magic_mask = 0x100010001000100;
*         u64 fst = (magic * (u64)bits) & magic_mask;
*         u64 snd = ((magic >> 7 ) * (u64)bits);
*         return fst | snd;
* }
* 
*/
_fb_fast_tile_byte:
        ubfiz   x1, x0, 14, 8
        add     x1, x1, w0, uxtb
        add     x1, x1, x1, lsl 28
        and     x0, x1, 281479271743489
        lsl     x1, x1, 7
        and     x1, x1, 72058693566333184
        orr     x0, x1, x0
        ret
