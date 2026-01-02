/*
 * RISC-V 64 multiplex routines
 *
 * Process scheduling and context switching
 */

#include "include/sconst.h"
#include "include/archconst.h"

    .section .text
    .align 2

/*
 * Context switch structure offsets
 * These must match struct context in proc.h
 */
#define CTX_RA      (0 * 8)
#define CTX_SP      (1 * 8)
#define CTX_S0      (2 * 8)
#define CTX_S1      (3 * 8)
#define CTX_S2      (4 * 8)
#define CTX_S3      (5 * 8)
#define CTX_S4      (6 * 8)
#define CTX_S5      (7 * 8)
#define CTX_S6      (8 * 8)
#define CTX_S7      (9 * 8)
#define CTX_S8      (10 * 8)
#define CTX_S9      (11 * 8)
#define CTX_S10     (12 * 8)
#define CTX_S11     (13 * 8)
#define CTX_SIZE    (14 * 8)

/*
 * void context_switch(struct context *old, struct context *new)
 *
 * Save current context to 'old' and restore from 'new'
 * This is for kernel-to-kernel context switches
 */
    .globl context_switch
    .type context_switch, @function
context_switch:
    /* Save callee-saved registers to old context */
    sd      ra, CTX_RA(a0)
    sd      sp, CTX_SP(a0)
    sd      s0, CTX_S0(a0)
    sd      s1, CTX_S1(a0)
    sd      s2, CTX_S2(a0)
    sd      s3, CTX_S3(a0)
    sd      s4, CTX_S4(a0)
    sd      s5, CTX_S5(a0)
    sd      s6, CTX_S6(a0)
    sd      s7, CTX_S7(a0)
    sd      s8, CTX_S8(a0)
    sd      s9, CTX_S9(a0)
    sd      s10, CTX_S10(a0)
    sd      s11, CTX_S11(a0)

    /* Restore callee-saved registers from new context */
    ld      ra, CTX_RA(a1)
    ld      sp, CTX_SP(a1)
    ld      s0, CTX_S0(a1)
    ld      s1, CTX_S1(a1)
    ld      s2, CTX_S2(a1)
    ld      s3, CTX_S3(a1)
    ld      s4, CTX_S4(a1)
    ld      s5, CTX_S5(a1)
    ld      s6, CTX_S6(a1)
    ld      s7, CTX_S7(a1)
    ld      s8, CTX_S8(a1)
    ld      s9, CTX_S9(a1)
    ld      s10, CTX_S10(a1)
    ld      s11, CTX_S11(a1)

    ret

/*
 * void context_init(struct context *ctx, void *stack, void (*func)(void *), void *arg)
 *
 * Initialize a context for first switch
 */
    .globl context_init
    .type context_init, @function
context_init:
    /* Set return address to function */
    sd      a2, CTX_RA(a0)

    /* Set stack pointer */
    sd      a1, CTX_SP(a0)

    /* Set s0 to argument (will be passed in a0 when function is called) */
    sd      a3, CTX_S0(a0)

    /* Clear other saved registers */
    sd      zero, CTX_S1(a0)
    sd      zero, CTX_S2(a0)
    sd      zero, CTX_S3(a0)
    sd      zero, CTX_S4(a0)
    sd      zero, CTX_S5(a0)
    sd      zero, CTX_S6(a0)
    sd      zero, CTX_S7(a0)
    sd      zero, CTX_S8(a0)
    sd      zero, CTX_S9(a0)
    sd      zero, CTX_S10(a0)
    sd      zero, CTX_S11(a0)

    ret

/*
 * void switch_address_space(phys_bytes pgdir)
 *
 * Switch to a different address space
 */
    .globl switch_address_space
    .type switch_address_space, @function
switch_address_space:
    /* Convert physical address to SATP value */
    srli    a0, a0, 12          /* PPN = addr >> 12 */
    li      t0, SATP_MODE_SV39
    or      a0, a0, t0
    csrw    CSR_SATP, a0
    .word   RISCV_SFENCE_VMA_INSN   /* Flush TLB */
    ret

/*
 * void flush_tlb(void)
 */
    .globl flush_tlb
    .type flush_tlb, @function
flush_tlb:
    .word   RISCV_SFENCE_VMA_INSN
    ret

/*
 * void flush_tlb_addr(vir_bytes addr)
 */
    .globl flush_tlb_addr
    .type flush_tlb_addr, @function
flush_tlb_addr:
    .word   RISCV_SFENCE_VMA_INSN
    ret

/*
 * void enable_interrupts(void)
 */
    .globl enable_interrupts
    .type enable_interrupts, @function
enable_interrupts:
    csrsi   CSR_SSTATUS, SSTATUS_SIE
    ret

/*
 * void disable_interrupts(void)
 */
    .globl disable_interrupts
    .type disable_interrupts, @function
disable_interrupts:
    csrci   CSR_SSTATUS, SSTATUS_SIE
    ret

/*
 * int interrupts_enabled(void)
 */
    .globl interrupts_enabled
    .type interrupts_enabled, @function
interrupts_enabled:
    csrr    t0, CSR_SSTATUS
    andi    a0, t0, SSTATUS_SIE
    snez    a0, a0
    ret

/*
 * u64_t disable_and_save_interrupts(void)
 *
 * Disable interrupts and return previous state
 */
    .globl disable_and_save_interrupts
    .type disable_and_save_interrupts, @function
disable_and_save_interrupts:
    csrrci  a0, CSR_SSTATUS, SSTATUS_SIE
    andi    a0, a0, SSTATUS_SIE
    ret

/*
 * void restore_interrupts(u64_t state)
 *
 * Restore interrupt state
 */
    .globl restore_interrupts
    .type restore_interrupts, @function
restore_interrupts:
    beqz    a0, 1f
    csrsi   CSR_SSTATUS, SSTATUS_SIE
    ret
1:
    ret

/*
 * Memory barriers
 */
    .globl mb
    .type mb, @function
mb:
    fence   iorw, iorw
    ret

    .globl rmb
    .type rmb, @function
rmb:
    fence   ir, ir
    ret

    .globl wmb
    .type wmb, @function
wmb:
    fence   ow, ow
    ret

    .globl smp_mb
    .type smp_mb, @function
smp_mb:
    fence   rw, rw
    ret
