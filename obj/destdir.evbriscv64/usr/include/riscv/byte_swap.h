/*	$NetBSD$	*/

#ifndef _RISCV_BYTE_SWAP_H_
#define _RISCV_BYTE_SWAP_H_

#ifdef _LOCORE

#else

#include <sys/types.h>

static __inline __uint16_t
__byte_swap_u16_variable(__uint16_t v)
{
	__uint16_t t = v;
	return (t >> 8) | (t << 8);
}

static __inline __uint32_t
__byte_swap_u32_variable(__uint32_t v)
{
	__uint32_t t = v;
	t = ((t & 0x00ff00ffU) << 8) | ((t >> 8) & 0x00ff00ffU);
	return (t >> 16) | (t << 16);
}

static __inline __uint64_t
__byte_swap_u64_variable(__uint64_t v)
{
	__uint64_t t = v;
	t = ((t & 0x00ff00ff00ff00ffULL) << 8) |
	    ((t >> 8) & 0x00ff00ff00ff00ffULL);
	t = ((t & 0x0000ffff0000ffffULL) << 16) |
	    ((t >> 16) & 0x0000ffff0000ffffULL);
	return (t >> 32) | (t << 32);
}

#define __BYTE_SWAP_U16_VARIABLE	__byte_swap_u16_variable
#define __BYTE_SWAP_U32_VARIABLE	__byte_swap_u32_variable
#define __BYTE_SWAP_U64_VARIABLE	__byte_swap_u64_variable

#endif /* _LOCORE */

#endif /* _RISCV_BYTE_SWAP_H_ */
