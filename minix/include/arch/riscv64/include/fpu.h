/*
 * RISC-V 64 FPU definitions
 */

#ifndef _RISCV64_FPU_H
#define _RISCV64_FPU_H

/* FPU state structure */
struct fpu_state {
    double      f[32];      /* Floating point registers f0-f31 */
    unsigned long fcsr;     /* FP control and status register */
};

/* FPU control flags in sstatus */
#define SSTATUS_FS_MASK     (3UL << 13)
#define SSTATUS_FS_OFF      (0UL << 13)
#define SSTATUS_FS_INITIAL  (1UL << 13)
#define SSTATUS_FS_CLEAN    (2UL << 13)
#define SSTATUS_FS_DIRTY    (3UL << 13)

/* FCSR bits */
#define FCSR_NX             (1 << 0)    /* Inexact */
#define FCSR_UF             (1 << 1)    /* Underflow */
#define FCSR_OF             (1 << 2)    /* Overflow */
#define FCSR_DZ             (1 << 3)    /* Divide by zero */
#define FCSR_NV             (1 << 4)    /* Invalid operation */
#define FCSR_FLAGS_MASK     0x1F

/* Rounding modes */
#define FCSR_RM_RNE         (0 << 5)    /* Round to nearest, ties to even */
#define FCSR_RM_RTZ         (1 << 5)    /* Round towards zero */
#define FCSR_RM_RDN         (2 << 5)    /* Round down (towards -inf) */
#define FCSR_RM_RUP         (3 << 5)    /* Round up (towards +inf) */
#define FCSR_RM_RMM         (4 << 5)    /* Round to nearest, ties to max magnitude */
#define FCSR_RM_MASK        (7 << 5)

#endif /* _RISCV64_FPU_H */
