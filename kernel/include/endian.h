#ifndef ENDIAN_H
#define ENDIAN_H 1
/* 
 * Functions for converting endianess 
 * at the moment im assuming that the chip will always be little endian
 */
#include <types.h>

static inline u32 beth32(const u32 *p)
{

	const u8 *bp = (const u8 *)p;
	return ((u32) bp[0] << 24)
	    | ((u32) bp[1] << 16)
	    | ((u32) bp[2] << 8)
	    | bp[3];
}

static inline u16 beth16(const u16 *p)
{
	const u8 *bp = (const u8 *)p;

	return ((u16) bp[0] << 8) | bp[1];
}

static inline u64 beth64(const u64 *p)
{

	const u8 *bp = (const u8 *)p;
	return ((u64) bp[0] << 56)
	    | ((u64) bp[1] << 48)
	    | ((u64) bp[2] << 40)
	    | ((u64) bp[3] << 32)
	    | ((u64) bp[4] << 24)
	    | ((u64) bp[5] << 16)
	    | ((u64) bp[6] << 8)
	    | bp[7];
}
#endif
