/*
 * RISC-V 64 ucontext manipulation
 *
 * Functions for manipulating user contexts (used for signals, coroutines, etc.)
 */

#include <machine/asm.h>
#include <ucontextoffsets.h>

/*
 * int getcontext(ucontext_t *ucp)
 *
 * Save current context
 */
ENTRY(getcontext)
ENTRY(_getcontext)
    /* a0 = ucp */

    /* Save return address as PC */
    sd      ra, REG_PC(a0)

    /* Save callee-saved registers */
    sd      ra, REG_RA(a0)
    sd      sp, REG_SP(a0)
    sd      gp, REG_GP(a0)
    sd      tp, REG_TP(a0)
    sd      s0, REG_S0(a0)
    sd      s1, REG_S1(a0)
    sd      s2, REG_S2(a0)
    sd      s3, REG_S3(a0)
    sd      s4, REG_S4(a0)
    sd      s5, REG_S5(a0)
    sd      s6, REG_S6(a0)
    sd      s7, REG_S7(a0)
    sd      s8, REG_S8(a0)
    sd      s9, REG_S9(a0)
    sd      s10, REG_S10(a0)
    sd      s11, REG_S11(a0)

    /* Save a0 as 0 (return value when context is restored) */
    sd      zero, REG_A0(a0)

    li      t0, MCF_MAGIC
    sw      t0, MAGIC(a0)

    /* Return 0 */
    li      a0, 0
    ret
END(getcontext)

/*
 * int setcontext(const ucontext_t *ucp)
 *
 * Restore saved context (does not return)
 */
ENTRY(setcontext)
    /* a0 = ucp */

    /* Restore callee-saved registers */
    ld      s0, REG_S0(a0)
    ld      s1, REG_S1(a0)
    ld      s2, REG_S2(a0)
    ld      s3, REG_S3(a0)
    ld      s4, REG_S4(a0)
    ld      s5, REG_S5(a0)
    ld      s6, REG_S6(a0)
    ld      s7, REG_S7(a0)
    ld      s8, REG_S8(a0)
    ld      s9, REG_S9(a0)
    ld      s10, REG_S10(a0)
    ld      s11, REG_S11(a0)

    /* Restore GP, TP */
    ld      gp, REG_GP(a0)
    ld      tp, REG_TP(a0)

    /* Restore RA and SP */
    ld      ra, REG_RA(a0)
    ld      sp, REG_SP(a0)

    /* Load target PC before clobbering a0 */
    ld      t0, REG_PC(a0)

    /* Load return value into a0 */
    ld      a0, REG_A0(a0)

    /* Jump to saved PC */
    jr      t0
END(setcontext)

/*
 * int swapcontext(ucontext_t *oucp, const ucontext_t *ucp)
 *
 * Save current context and switch to new one
 */
ENTRY(swapcontext)
    /* a0 = oucp, a1 = ucp */

    /* Save current context to oucp */
    sd      ra, REG_PC(a0)
    sd      ra, REG_RA(a0)
    sd      sp, REG_SP(a0)
    sd      gp, REG_GP(a0)
    sd      tp, REG_TP(a0)
    sd      s0, REG_S0(a0)
    sd      s1, REG_S1(a0)
    sd      s2, REG_S2(a0)
    sd      s3, REG_S3(a0)
    sd      s4, REG_S4(a0)
    sd      s5, REG_S5(a0)
    sd      s6, REG_S6(a0)
    sd      s7, REG_S7(a0)
    sd      s8, REG_S8(a0)
    sd      s9, REG_S9(a0)
    sd      s10, REG_S10(a0)
    sd      s11, REG_S11(a0)
    sd      zero, REG_A0(a0)  /* Return 0 when restored */

    li      t0, MCF_MAGIC
    sw      t0, MAGIC(a0)

    /* Now restore from ucp (a1) */
    mv      a0, a1

    /* Restore callee-saved registers */
    ld      s0, REG_S0(a0)
    ld      s1, REG_S1(a0)
    ld      s2, REG_S2(a0)
    ld      s3, REG_S3(a0)
    ld      s4, REG_S4(a0)
    ld      s5, REG_S5(a0)
    ld      s6, REG_S6(a0)
    ld      s7, REG_S7(a0)
    ld      s8, REG_S8(a0)
    ld      s9, REG_S9(a0)
    ld      s10, REG_S10(a0)
    ld      s11, REG_S11(a0)

    ld      gp, REG_GP(a0)
    ld      tp, REG_TP(a0)
    ld      ra, REG_RA(a0)
    ld      sp, REG_SP(a0)

    ld      t0, REG_PC(a0)
    ld      a0, REG_A0(a0)

    jr      t0
END(swapcontext)
