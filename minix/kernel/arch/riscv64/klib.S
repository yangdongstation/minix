/*
 * RISC-V 64 kernel library routines
 *
 * Low-level assembly routines for context switching, FPU, etc.
 */

#include "include/sconst.h"
#include "include/archconst.h"

    .section .text
    .align 2

/*
 * void restore_user_context(struct proc *p)
 *
 * Restore user context from process structure and return to user mode.
 *
 * a0 = pointer to proc structure (which starts with trapframe)
 */
    .globl restore_user_context
    .type restore_user_context, @function
restore_user_context:
    /* Set sscratch to kernel stack for next trap */
    /* TODO: Calculate proper kernel stack from proc */
    csrw    sscratch, sp

    /* Load trapframe pointer */
    mv      sp, a0

    /* Restore CSRs */
    ld      t0, TF_SEPC(sp)
    csrw    sepc, t0
    ld      t0, TF_SSTATUS(sp)
    csrw    sstatus, t0

    /* Restore general registers */
    ld      ra, TF_RA(sp)
    ld      gp, TF_GP(sp)
    ld      tp, TF_TP(sp)
    ld      t0, TF_T0(sp)
    ld      t1, TF_T1(sp)
    ld      t2, TF_T2(sp)
    ld      s0, TF_S0(sp)
    ld      s1, TF_S1(sp)
    ld      a0, TF_A0(sp)
    ld      a1, TF_A1(sp)
    ld      a2, TF_A2(sp)
    ld      a3, TF_A3(sp)
    ld      a4, TF_A4(sp)
    ld      a5, TF_A5(sp)
    ld      a6, TF_A6(sp)
    ld      a7, TF_A7(sp)
    ld      s2, TF_S2(sp)
    ld      s3, TF_S3(sp)
    ld      s4, TF_S4(sp)
    ld      s5, TF_S5(sp)
    ld      s6, TF_S6(sp)
    ld      s7, TF_S7(sp)
    ld      s8, TF_S8(sp)
    ld      s9, TF_S9(sp)
    ld      s10, TF_S10(sp)
    ld      s11, TF_S11(sp)
    ld      t3, TF_T3(sp)
    ld      t4, TF_T4(sp)
    ld      t5, TF_T5(sp)
    ld      t6, TF_T6(sp)

    /* Restore user sp last */
    ld      sp, TF_SP(sp)

    sret

/*
 * void save_fpu(struct proc *p)
 *
 * Save FPU state to process structure
 */
    .globl save_fpu
    .type save_fpu, @function
save_fpu:
    /* Check if FPU is enabled */
    csrr    t0, sstatus
    li      t1, SSTATUS_FS_MASK
    and     t0, t0, t1
    beqz    t0, .Lsave_fpu_done

    /* TODO: Save F registers */
    /* fsd f0-f31 to proc structure */

.Lsave_fpu_done:
    ret

/*
 * int restore_fpu(struct proc *p)
 *
 * Restore FPU state from process structure
 */
    .globl restore_fpu
    .type restore_fpu, @function
restore_fpu:
    /* Enable FPU */
    li      t0, SSTATUS_FS_INITIAL
    csrs    sstatus, t0

    /* TODO: Restore F registers */
    /* fld f0-f31 from proc structure */

    /* Return OK */
    li      a0, 0
    ret

/*
 * void level0(void (*func)(void))
 *
 * Execute function at kernel privilege level
 * On RISC-V we're already in S-mode, just call it
 */
    .globl level0
    .type level0, @function
level0:
    jalr    a0
    ret

/*
 * int cpu_number(void)
 *
 * Get current CPU (hart) ID
 */
    .globl cpu_number
    .type cpu_number, @function
cpu_number:
    /* Hart ID stored in tp during boot */
    mv      a0, tp
    ret

/*
 * void halt_cpu(void)
 *
 * Halt the CPU
 */
    .globl halt_cpu
    .type halt_cpu, @function
halt_cpu:
    csrci   sstatus, SSTATUS_SIE  /* Disable interrupts */
.Lhalt_loop:
    wfi
    j       .Lhalt_loop
