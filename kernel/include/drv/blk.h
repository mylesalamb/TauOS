#ifndef BLK_H
#define BLK_H 1

#include <types.h>

struct blk_dev 
{
    void (*read)(void *, u64); 
    void (*write)(void *, u64);
    void (*seek)(u64);
};

#endif