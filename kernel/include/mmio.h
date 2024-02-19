#ifndef MMIO_H
#define MMIO_H 1

#include <types.h>

/* 
General purpose MMIO definitions, we dont want the compiler to optimise out the calls
and we dont want it to do a full function call / teardown for what really is a macro
*/
static void inline
    __attribute__((always_inline)) mmio_write64(uintptr_t r, u64 v)
{
	*(volatile u64 *)r = v;
}

static void inline
    __attribute__((always_inline)) mmio_write32(uintptr_t r, u32 v)
{
	*(volatile u32 *)r = v;
}

static void inline
    __attribute__((always_inline)) mmio_write16(uintptr_t r, u16 v)
{
	*(volatile u16 *)r = v;
}

static void inline __attribute__((always_inline)) mmio_write8(uintptr_t r, u8 v)
{
	*(volatile u8 *)r = v;
}

static u64 inline __attribute__((always_inline)) mmio_read64(uintptr_t r)
{
	return *(volatile u64 *)r;
}

static u32 inline __attribute__((always_inline)) mmio_read32(uintptr_t r)
{
	return *(volatile u32 *)r;
}

static u16 inline __attribute__((always_inline)) mmio_read16(uintptr_t r)
{
	return *(volatile u16 *)r;
}

static u8 inline __attribute__((always_inline)) mmio_read8(uintptr_t r)
{
	return *(volatile u8 *)r;
}
#endif
