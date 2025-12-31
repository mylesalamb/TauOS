#ifndef KMOD_H
#define KMOD_H 1

#define kmod_init(fn)                                      \
    static int (*__module_init_##fn)(void)                 \
    __attribute__((used, section(".modinit"))) = fn

#endif
