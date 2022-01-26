#ifndef BITMAP_H
#define BITMAP_H 1

#include <types.h>
struct bitmap {
        u64 _bits;
};

#define BITMAP_INIT(name, sz) (struct bitmap name[ (sz / (sizeof(struct bitmap) * 8)) + ( sz % sizeof(struct bitmap) * 8 ) ? 1 : 0 ] )

void bitmap_set_bit(struct bitmap *, u64);
void bitmap_clr_bit(struct bitmap *, u64);
u64 bitmap_is_set(struct bitmap *, u64);
u64 bitmap_get_size(u64);
#endif
