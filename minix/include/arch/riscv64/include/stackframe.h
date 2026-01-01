/*
 * RISC-V 64 stack frame definitions
 *
 * This structure is used for saving process state during traps
 */

#ifndef _RISCV64_STACKFRAME_H
#define _RISCV64_STACKFRAME_H

#include <sys/types.h>

typedef unsigned long reg_t;		/* machine register */
typedef reg_t segdesc_t;

/* Stack frame structure - matches trapframe in kernel */
struct stackframe_s {
    /* General purpose registers (x0-x31) */
    reg_t ra;           /* x1: Return address */
    reg_t sp;           /* x2: Stack pointer */
    reg_t gp;           /* x3: Global pointer */
    reg_t tp;           /* x4: Thread pointer */
    reg_t t0;           /* x5: Temporary */
    reg_t t1;           /* x6: Temporary */
    reg_t t2;           /* x7: Temporary */
    reg_t s0;           /* x8: Saved register / Frame pointer */
    reg_t s1;           /* x9: Saved register */
    reg_t a0;           /* x10: Argument 0 / Return value */
    reg_t a1;           /* x11: Argument 1 / Return value */
    reg_t a2;           /* x12: Argument 2 */
    reg_t a3;           /* x13: Argument 3 */
    reg_t a4;           /* x14: Argument 4 */
    reg_t a5;           /* x15: Argument 5 */
    reg_t a6;           /* x16: Argument 6 */
    reg_t a7;           /* x17: Argument 7 / Syscall number */
    reg_t s2;           /* x18: Saved register */
    reg_t s3;           /* x19: Saved register */
    reg_t s4;           /* x20: Saved register */
    reg_t s5;           /* x21: Saved register */
    reg_t s6;           /* x22: Saved register */
    reg_t s7;           /* x23: Saved register */
    reg_t s8;           /* x24: Saved register */
    reg_t s9;           /* x25: Saved register */
    reg_t s10;          /* x26: Saved register */
    reg_t s11;          /* x27: Saved register */
    reg_t t3;           /* x28: Temporary */
    reg_t t4;           /* x29: Temporary */
    reg_t t5;           /* x30: Temporary */
    reg_t t6;           /* x31: Temporary */

    /* CSRs saved on trap */
    reg_t pc;           /* sepc: Exception program counter */
    reg_t psr;          /* sstatus: Status register */
    reg_t cause;        /* scause: Exception cause */
    reg_t tval;         /* stval: Trap value (fault address, etc.) */
};

/* Offsets for assembly code */
#define SF_RA       (0 * 8)
#define SF_SP       (1 * 8)
#define SF_GP       (2 * 8)
#define SF_TP       (3 * 8)
#define SF_T0       (4 * 8)
#define SF_T1       (5 * 8)
#define SF_T2       (6 * 8)
#define SF_S0       (7 * 8)
#define SF_S1       (8 * 8)
#define SF_A0       (9 * 8)
#define SF_A1       (10 * 8)
#define SF_A2       (11 * 8)
#define SF_A3       (12 * 8)
#define SF_A4       (13 * 8)
#define SF_A5       (14 * 8)
#define SF_A6       (15 * 8)
#define SF_A7       (16 * 8)
#define SF_S2       (17 * 8)
#define SF_S3       (18 * 8)
#define SF_S4       (19 * 8)
#define SF_S5       (20 * 8)
#define SF_S6       (21 * 8)
#define SF_S7       (22 * 8)
#define SF_S8       (23 * 8)
#define SF_S9       (24 * 8)
#define SF_S10      (25 * 8)
#define SF_S11      (26 * 8)
#define SF_T3       (27 * 8)
#define SF_T4       (28 * 8)
#define SF_T5       (29 * 8)
#define SF_T6       (30 * 8)
#define SF_PC       (31 * 8)
#define SF_PSR      (32 * 8)
#define SF_CAUSE    (33 * 8)
#define SF_TVAL     (34 * 8)

#define STACKFRAME_SIZE (35 * 8)

#endif /* _RISCV64_STACKFRAME_H */
