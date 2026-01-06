/*
 * RISC-V 64 physical memory copy routines
 *
 * These routines copy data between physical addresses,
 * handling potential page faults gracefully.
 */

#include "include/sconst.h"
#include "include/archconst.h"

#define UART_THR        0x00
#define UART_LSR        0x05
#define UART_LSR_THRE   0x20

    .macro UART_PUTC reg
    li      t5, VIRT_UART0_BASE
1:
    lbu     t6, UART_LSR(t5)
    andi    t6, t6, UART_LSR_THRE
    beqz    t6, 1b
    sb      \reg, UART_THR(t5)
    .endm

    .section .text
    .align 2

    .section .bss
    .align 3
phys_copy_trace_once:
    .quad 0

    .section .text
    .align 2

/*
 * phys_bytes phys_copy(phys_bytes src, phys_bytes dst, phys_bytes size)
 *
 * Copy size bytes from src to dst.
 * Returns 0 on success, non-zero on failure (page fault).
 *
 * Arguments:
 *   a0 = source physical address
 *   a1 = destination physical address
 *   a2 = number of bytes to copy
 */
    .globl phys_copy
    .type phys_copy, @function
phys_copy:
    /* Save return address */
    addi    sp, sp, -16
    sd      ra, 8(sp)
    sd      s0, 0(sp)
    li      s0, 0

    /* One-time trace for a typical 1KB kernel->VM copy. */
    li      t3, 0x400
    bne     a2, t3, .Ltrace_done
    la      t3, phys_copy_trace_once
    ld      t4, 0(t3)
    bnez    t4, .Ltrace_done
    li      t4, 1
    sd      t4, 0(t3)
    li      s0, 1
    li      t4, 'C'
    UART_PUTC t4
.Ltrace_done:

    /* Check for zero length */
    beqz    a2, .Lcopy_done

    /* Translate physical addresses to kernel virtual */
    li      t0, VIRT_DRAM_BASE
    bltu    a0, t0, 1f
    li      t1, KERNEL_BASE
    sub     a0, a0, t0
    add     a0, a0, t1
1:
    bltu    a1, t0, 2f
    li      t1, KERNEL_BASE
    sub     a1, a1, t0
    add     a1, a1, t1
2:

    /* Set up fault handler */
    la      t0, .Lcopy_fault
    /* TODO: Register fault handler */

    /* Align check - if both aligned to 8 bytes, use doubleword copy */
    or      t0, a0, a1
    andi    t0, t0, 7
    bnez    t0, .Lcopy_byte

    /* Check if we can do 8-byte aligned copy */
    li      t0, 8
    bltu    a2, t0, .Lcopy_byte

.Lcopy_dword:
    /* Copy 8 bytes at a time */
    ld      t1, 0(a0)
    sd      t1, 0(a1)
    addi    a0, a0, 8
    addi    a1, a1, 8
    addi    a2, a2, -8
    bgeu    a2, t0, .Lcopy_dword

    /* Fall through to byte copy for remainder */
    beqz    a2, .Lcopy_done

.Lcopy_byte:
    /* Copy one byte at a time */
    lb      t0, 0(a0)
    sb      t0, 0(a1)
    addi    a0, a0, 1
    addi    a1, a1, 1
    addi    a2, a2, -1
    bnez    a2, .Lcopy_byte

.Lcopy_done:
    beqz    s0, .Lcopy_done_ret
    li      t4, 'c'
    UART_PUTC t4
.Lcopy_done_ret:
    /* Success - return 0 */
    li      a0, 0
    ld      s0, 0(sp)
    ld      ra, 8(sp)
    addi    sp, sp, 16
    ret

.Lcopy_fault:
    beqz    s0, .Lcopy_fault_ret
    li      t4, 'F'
    UART_PUTC t4
.Lcopy_fault_ret:
    /* Page fault during copy - return error */
    li      a0, -1
    ld      s0, 0(sp)
    ld      ra, 8(sp)
    addi    sp, sp, 16
    ret

/*
 * void phys_memset(phys_bytes dst, unsigned long c, phys_bytes size)
 *
 * Set size bytes at dst to value c.
 *
 * Arguments:
 *   a0 = destination physical address
 *   a1 = value (only low byte used)
 *   a2 = number of bytes to set
 */
    .globl phys_memset
    .type phys_memset, @function
phys_memset:
    /* Check for zero length */
    beqz    a2, .Lmemset_done

    /* Translate physical address to kernel virtual */
    li      t0, VIRT_DRAM_BASE
    bltu    a0, t0, 1f
    li      t1, KERNEL_BASE
    sub     a0, a0, t0
    add     a0, a0, t1
1:

    /* Create byte pattern */
    andi    a1, a1, 0xFF

    /* Check alignment for fast path */
    andi    t0, a0, 7
    bnez    t0, .Lmemset_byte

    /* Create doubleword pattern */
    slli    t0, a1, 8
    or      a1, a1, t0
    slli    t0, a1, 16
    or      a1, a1, t0
    slli    t0, a1, 32
    or      a1, a1, t0

    /* Check if we can do 8-byte aligned set */
    li      t1, 8
    bltu    a2, t1, .Lmemset_byte

.Lmemset_dword:
    /* Set 8 bytes at a time */
    sd      a1, 0(a0)
    addi    a0, a0, 8
    addi    a2, a2, -8
    bgeu    a2, t1, .Lmemset_dword

    /* Restore byte value for remainder */
    andi    a1, a1, 0xFF
    beqz    a2, .Lmemset_done

.Lmemset_byte:
    /* Set one byte at a time */
    sb      a1, 0(a0)
    addi    a0, a0, 1
    addi    a2, a2, -1
    bnez    a2, .Lmemset_byte

.Lmemset_done:
    ret

/*
 * Fault handler entry points (called from exception handler)
 */
    .globl phys_copy_fault
    .type phys_copy_fault, @function
phys_copy_fault:
    /* Jump to fault recovery */
    j       .Lcopy_fault

    .globl phys_copy_fault_in_kernel
    .type phys_copy_fault_in_kernel, @function
phys_copy_fault_in_kernel:
    /* Same as above for kernel faults */
    j       .Lcopy_fault

/*
 * void *memcpy(void *dst, const void *src, size_t n)
 *
 * Standard memcpy for kernel use
 */
    .globl memcpy
    .type memcpy, @function
memcpy:
    /* Save destination for return */
    mv      t2, a0

    beqz    a2, .Lmemcpy_done

    /* Simple byte copy */
.Lmemcpy_loop:
    lb      t0, 0(a1)
    sb      t0, 0(a0)
    addi    a0, a0, 1
    addi    a1, a1, 1
    addi    a2, a2, -1
    bnez    a2, .Lmemcpy_loop

.Lmemcpy_done:
    mv      a0, t2
    ret

/*
 * void *memset(void *s, int c, size_t n)
 *
 * Standard memset for kernel use
 */
    .globl memset
    .type memset, @function
memset:
    /* Save destination for return */
    mv      t2, a0

    beqz    a2, .Lset_done

    andi    a1, a1, 0xFF

.Lset_loop:
    sb      a1, 0(a0)
    addi    a0, a0, 1
    addi    a2, a2, -1
    bnez    a2, .Lset_loop

.Lset_done:
    mv      a0, t2
    ret

/*
 * int memcmp(const void *s1, const void *s2, size_t n)
 */
    .globl memcmp
    .type memcmp, @function
memcmp:
    beqz    a2, .Lcmp_equal

.Lcmp_loop:
    lbu     t0, 0(a0)
    lbu     t1, 0(a1)
    bne     t0, t1, .Lcmp_diff
    addi    a0, a0, 1
    addi    a1, a1, 1
    addi    a2, a2, -1
    bnez    a2, .Lcmp_loop

.Lcmp_equal:
    li      a0, 0
    ret

.Lcmp_diff:
    sub     a0, t0, t1
    ret
