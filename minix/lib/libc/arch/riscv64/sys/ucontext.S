/*
 * RISC-V 64 ucontext manipulation
 *
 * Functions for manipulating user contexts (used for signals, coroutines, etc.)
 */

#include <machine/asm.h>

/* Register offsets in mcontext_t (match sys/arch/riscv/include/mcontext.h) */
#define UC_GREGS    0
#define REG_PC      0
#define REG_RA      8
#define REG_SP      16
#define REG_GP      24
#define REG_TP      32
#define REG_T0      40
#define REG_T1      48
#define REG_T2      56
#define REG_S0      64
#define REG_S1      72
#define REG_A0      80
#define REG_A1      88
#define REG_A2      96
#define REG_A3      104
#define REG_A4      112
#define REG_A5      120
#define REG_A6      128
#define REG_A7      136
#define REG_S2      144
#define REG_S3      152
#define REG_S4      160
#define REG_S5      168
#define REG_S6      176
#define REG_S7      184
#define REG_S8      192
#define REG_S9      200
#define REG_S10     208
#define REG_S11     216
#define REG_T3      224
#define REG_T4      232
#define REG_T5      240
#define REG_T6      248

/* Size of general register save area */
#define GREGS_SIZE  256

/*
 * int getcontext(ucontext_t *ucp)
 *
 * Save current context
 */
ENTRY(getcontext)
    /* a0 = ucp */

    /* Save return address as PC */
    sd      ra, UC_GREGS+REG_PC(a0)

    /* Save callee-saved registers */
    sd      ra, UC_GREGS+REG_RA(a0)
    sd      sp, UC_GREGS+REG_SP(a0)
    sd      gp, UC_GREGS+REG_GP(a0)
    sd      tp, UC_GREGS+REG_TP(a0)
    sd      s0, UC_GREGS+REG_S0(a0)
    sd      s1, UC_GREGS+REG_S1(a0)
    sd      s2, UC_GREGS+REG_S2(a0)
    sd      s3, UC_GREGS+REG_S3(a0)
    sd      s4, UC_GREGS+REG_S4(a0)
    sd      s5, UC_GREGS+REG_S5(a0)
    sd      s6, UC_GREGS+REG_S6(a0)
    sd      s7, UC_GREGS+REG_S7(a0)
    sd      s8, UC_GREGS+REG_S8(a0)
    sd      s9, UC_GREGS+REG_S9(a0)
    sd      s10, UC_GREGS+REG_S10(a0)
    sd      s11, UC_GREGS+REG_S11(a0)

    /* Save a0 as 0 (return value when context is restored) */
    sd      zero, UC_GREGS+REG_A0(a0)

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
    ld      s0, UC_GREGS+REG_S0(a0)
    ld      s1, UC_GREGS+REG_S1(a0)
    ld      s2, UC_GREGS+REG_S2(a0)
    ld      s3, UC_GREGS+REG_S3(a0)
    ld      s4, UC_GREGS+REG_S4(a0)
    ld      s5, UC_GREGS+REG_S5(a0)
    ld      s6, UC_GREGS+REG_S6(a0)
    ld      s7, UC_GREGS+REG_S7(a0)
    ld      s8, UC_GREGS+REG_S8(a0)
    ld      s9, UC_GREGS+REG_S9(a0)
    ld      s10, UC_GREGS+REG_S10(a0)
    ld      s11, UC_GREGS+REG_S11(a0)

    /* Restore GP, TP */
    ld      gp, UC_GREGS+REG_GP(a0)
    ld      tp, UC_GREGS+REG_TP(a0)

    /* Restore RA and SP */
    ld      ra, UC_GREGS+REG_RA(a0)
    ld      sp, UC_GREGS+REG_SP(a0)

    /* Load return value into a0 */
    ld      a0, UC_GREGS+REG_A0(a0)

    /* Jump to saved PC (which is in ra) */
    ret
END(setcontext)

/*
 * int swapcontext(ucontext_t *oucp, const ucontext_t *ucp)
 *
 * Save current context and switch to new one
 */
ENTRY(swapcontext)
    /* a0 = oucp, a1 = ucp */

    /* Save current context to oucp */
    sd      ra, UC_GREGS+REG_PC(a0)
    sd      ra, UC_GREGS+REG_RA(a0)
    sd      sp, UC_GREGS+REG_SP(a0)
    sd      gp, UC_GREGS+REG_GP(a0)
    sd      tp, UC_GREGS+REG_TP(a0)
    sd      s0, UC_GREGS+REG_S0(a0)
    sd      s1, UC_GREGS+REG_S1(a0)
    sd      s2, UC_GREGS+REG_S2(a0)
    sd      s3, UC_GREGS+REG_S3(a0)
    sd      s4, UC_GREGS+REG_S4(a0)
    sd      s5, UC_GREGS+REG_S5(a0)
    sd      s6, UC_GREGS+REG_S6(a0)
    sd      s7, UC_GREGS+REG_S7(a0)
    sd      s8, UC_GREGS+REG_S8(a0)
    sd      s9, UC_GREGS+REG_S9(a0)
    sd      s10, UC_GREGS+REG_S10(a0)
    sd      s11, UC_GREGS+REG_S11(a0)
    sd      zero, UC_GREGS+REG_A0(a0)  /* Return 0 when restored */

    /* Now restore from ucp (a1) */
    mv      a0, a1

    /* Restore callee-saved registers */
    ld      s0, UC_GREGS+REG_S0(a0)
    ld      s1, UC_GREGS+REG_S1(a0)
    ld      s2, UC_GREGS+REG_S2(a0)
    ld      s3, UC_GREGS+REG_S3(a0)
    ld      s4, UC_GREGS+REG_S4(a0)
    ld      s5, UC_GREGS+REG_S5(a0)
    ld      s6, UC_GREGS+REG_S6(a0)
    ld      s7, UC_GREGS+REG_S7(a0)
    ld      s8, UC_GREGS+REG_S8(a0)
    ld      s9, UC_GREGS+REG_S9(a0)
    ld      s10, UC_GREGS+REG_S10(a0)
    ld      s11, UC_GREGS+REG_S11(a0)

    ld      gp, UC_GREGS+REG_GP(a0)
    ld      tp, UC_GREGS+REG_TP(a0)
    ld      ra, UC_GREGS+REG_RA(a0)
    ld      sp, UC_GREGS+REG_SP(a0)

    ld      a0, UC_GREGS+REG_A0(a0)

    ret
END(swapcontext)
