/*
 * RISC-V 64 kernel library routines
 *
 * Low-level assembly routines for context switching, FPU, etc.
 */

#include "include/sconst.h"
#include "include/archconst.h"
#include "kernel/procoffsets.h"

#define FPU_FCSR_OFFSET    (32 * 8)

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
    /* Reset kernel stack pointer for next trap. */
    beqz    tp, 1f
    la      t0, _secondary_stacks
    li      t1, STACK_SIZE
    mul     t2, tp, t1
    add     t0, t0, t2
    add     t0, t0, t1
    j       2f
1:
    la      t0, _boot_stack_top
2:
    csrw    CSR_SSCRATCH, t0

    /* Load trapframe pointer */
    mv      sp, a0

    /* Restore CSRs */
    ld      t0, TF_SEPC(sp)
    csrw    CSR_SEPC, t0
    ld      t0, TF_SSTATUS(sp)
    csrw    CSR_SSTATUS, t0

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

    .word   RISCV_SRET_INSN

/*
 * void save_fpu(struct proc *p)
 *
 * Save FPU state to process structure
 */
    .globl save_fpu
    .type save_fpu, @function
save_fpu:
    /* Check if FPU is enabled */
    csrr    t0, CSR_SSTATUS
    li      t1, SSTATUS_FS_MASK
    and     t0, t0, t1
    beqz    t0, .Lsave_fpu_done

    ld      t2, P_FPU_STATE(a0)
    beqz    t2, .Lsave_fpu_done

    /* Save F registers */
    fsd     f0, (0 * 8)(t2)
    fsd     f1, (1 * 8)(t2)
    fsd     f2, (2 * 8)(t2)
    fsd     f3, (3 * 8)(t2)
    fsd     f4, (4 * 8)(t2)
    fsd     f5, (5 * 8)(t2)
    fsd     f6, (6 * 8)(t2)
    fsd     f7, (7 * 8)(t2)
    fsd     f8, (8 * 8)(t2)
    fsd     f9, (9 * 8)(t2)
    fsd     f10, (10 * 8)(t2)
    fsd     f11, (11 * 8)(t2)
    fsd     f12, (12 * 8)(t2)
    fsd     f13, (13 * 8)(t2)
    fsd     f14, (14 * 8)(t2)
    fsd     f15, (15 * 8)(t2)
    fsd     f16, (16 * 8)(t2)
    fsd     f17, (17 * 8)(t2)
    fsd     f18, (18 * 8)(t2)
    fsd     f19, (19 * 8)(t2)
    fsd     f20, (20 * 8)(t2)
    fsd     f21, (21 * 8)(t2)
    fsd     f22, (22 * 8)(t2)
    fsd     f23, (23 * 8)(t2)
    fsd     f24, (24 * 8)(t2)
    fsd     f25, (25 * 8)(t2)
    fsd     f26, (26 * 8)(t2)
    fsd     f27, (27 * 8)(t2)
    fsd     f28, (28 * 8)(t2)
    fsd     f29, (29 * 8)(t2)
    fsd     f30, (30 * 8)(t2)
    fsd     f31, (31 * 8)(t2)

    csrr    t0, CSR_FCSR
    sd      t0, FPU_FCSR_OFFSET(t2)

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
    csrs    CSR_SSTATUS, t0

    ld      t2, P_FPU_STATE(a0)
    beqz    t2, .Lrestore_fpu_fail

    /* Restore F registers */
    fld     f0, (0 * 8)(t2)
    fld     f1, (1 * 8)(t2)
    fld     f2, (2 * 8)(t2)
    fld     f3, (3 * 8)(t2)
    fld     f4, (4 * 8)(t2)
    fld     f5, (5 * 8)(t2)
    fld     f6, (6 * 8)(t2)
    fld     f7, (7 * 8)(t2)
    fld     f8, (8 * 8)(t2)
    fld     f9, (9 * 8)(t2)
    fld     f10, (10 * 8)(t2)
    fld     f11, (11 * 8)(t2)
    fld     f12, (12 * 8)(t2)
    fld     f13, (13 * 8)(t2)
    fld     f14, (14 * 8)(t2)
    fld     f15, (15 * 8)(t2)
    fld     f16, (16 * 8)(t2)
    fld     f17, (17 * 8)(t2)
    fld     f18, (18 * 8)(t2)
    fld     f19, (19 * 8)(t2)
    fld     f20, (20 * 8)(t2)
    fld     f21, (21 * 8)(t2)
    fld     f22, (22 * 8)(t2)
    fld     f23, (23 * 8)(t2)
    fld     f24, (24 * 8)(t2)
    fld     f25, (25 * 8)(t2)
    fld     f26, (26 * 8)(t2)
    fld     f27, (27 * 8)(t2)
    fld     f28, (28 * 8)(t2)
    fld     f29, (29 * 8)(t2)
    fld     f30, (30 * 8)(t2)
    fld     f31, (31 * 8)(t2)

    ld      t0, FPU_FCSR_OFFSET(t2)
    csrw    CSR_FCSR, t0

    lw      t0, P_MISC_FLAGS(a0)
    li      t1, MF_FPU_INITIALIZED
    or      t0, t0, t1
    sw      t0, P_MISC_FLAGS(a0)

    /* Return OK */
    li      a0, 0
    ret

.Lrestore_fpu_fail:
    li      a0, 1
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
    /* Enable interrupts while waiting, return with interrupts disabled. */
    csrsi   CSR_SSTATUS, SSTATUS_SIE
    .word   RISCV_WFI_INSN
    csrci   CSR_SSTATUS, SSTATUS_SIE
    ret
