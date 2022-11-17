#include <lib/bitmap.h>

void bitmap_set_bit(struct bitmap *map, u64 bit) {
        u64 block_sz = sizeof(map->_bits) * 8;
        u64 block = bit / block_sz;
        u64 cell = 1 << (bit % block_sz);
        map[block]._bits |=  cell;
}

void bitmap_clr_bit(struct bitmap *map, u64 bit) {
        u64 block_sz = sizeof(map->_bits) * 8;
        u64 block = bit / block_sz;
        u64 mask = ~(1 << (bit % block_sz));
        map[block]._bits &= mask;
}

u64 bitmap_is_set(struct bitmap *map, u64 bit) {
        u64 block_sz = sizeof(map->_bits) * 8;
        u64 block = bit / block_sz;
        u64 cell = 1 << (bit % block_sz);
        return map[block]._bits & cell;
}

u64 bitmap_get_size(u64 bits) {
       return (bits / (sizeof(struct bitmap) * 8)) + ( bits % sizeof(struct bitmap) * 8 ) ? 1 : 0; 
}
