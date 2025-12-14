/*
 * RISC-V 64 physical memory set routine
 *
 * Alternative implementation in assembly for performance
 * The main implementation is in phys_copy.S
 */

#include "include/sconst.h"

    .section .text
    .align 2

/*
 * void phys_memset_asm(phys_bytes dst, unsigned long pattern, phys_bytes size)
 *
 * Fill memory with a pattern (optimized version)
 *
 * Arguments:
 *   a0 = destination physical address
 *   a1 = pattern (64-bit)
 *   a2 = number of bytes to set
 */
    .globl phys_memset_asm
    .type phys_memset_asm, @function
phys_memset_asm:
    /* Check for zero length */
    beqz    a2, .Lpmemset_done

    /* Check alignment */
    andi    t0, a0, 7
    bnez    t0, .Lpmemset_byte

    /* Check if we have at least 64 bytes for unrolled loop */
    li      t0, 64
    bltu    a2, t0, .Lpmemset_dword

.Lpmemset_unroll:
    /* Set 64 bytes at a time (8 doublewords) */
    sd      a1, 0(a0)
    sd      a1, 8(a0)
    sd      a1, 16(a0)
    sd      a1, 24(a0)
    sd      a1, 32(a0)
    sd      a1, 40(a0)
    sd      a1, 48(a0)
    sd      a1, 56(a0)
    addi    a0, a0, 64
    addi    a2, a2, -64
    bgeu    a2, t0, .Lpmemset_unroll

    /* Handle remaining doublewords */
    beqz    a2, .Lpmemset_done

.Lpmemset_dword:
    li      t0, 8
    bltu    a2, t0, .Lpmemset_byte

.Lpmemset_dword_loop:
    sd      a1, 0(a0)
    addi    a0, a0, 8
    addi    a2, a2, -8
    bgeu    a2, t0, .Lpmemset_dword_loop

    beqz    a2, .Lpmemset_done

.Lpmemset_byte:
    /* Set remaining bytes */
    andi    t1, a1, 0xFF
.Lpmemset_byte_loop:
    sb      t1, 0(a0)
    addi    a0, a0, 1
    addi    a2, a2, -1
    bnez    a2, .Lpmemset_byte_loop

.Lpmemset_done:
    ret

/*
 * void phys_zero(phys_bytes dst, phys_bytes size)
 *
 * Zero memory region (optimized)
 */
    .globl phys_zero
    .type phys_zero, @function
phys_zero:
    /* Call phys_memset_asm with zero pattern */
    mv      a2, a1
    li      a1, 0
    j       phys_memset_asm
