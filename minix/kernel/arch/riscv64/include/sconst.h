/*
 * RISC-V 64 assembly constants for trapframe
 */

#ifndef _RISCV64_SCONST_H
#define _RISCV64_SCONST_H

/* Trapframe register offsets (8 bytes each for RV64) */
#define TF_RA       (0 * 8)
#define TF_SP       (1 * 8)
#define TF_GP       (2 * 8)
#define TF_TP       (3 * 8)
#define TF_T0       (4 * 8)
#define TF_T1       (5 * 8)
#define TF_T2       (6 * 8)
#define TF_S0       (7 * 8)
#define TF_S1       (8 * 8)
#define TF_A0       (9 * 8)
#define TF_A1       (10 * 8)
#define TF_A2       (11 * 8)
#define TF_A3       (12 * 8)
#define TF_A4       (13 * 8)
#define TF_A5       (14 * 8)
#define TF_A6       (15 * 8)
#define TF_A7       (16 * 8)
#define TF_S2       (17 * 8)
#define TF_S3       (18 * 8)
#define TF_S4       (19 * 8)
#define TF_S5       (20 * 8)
#define TF_S6       (21 * 8)
#define TF_S7       (22 * 8)
#define TF_S8       (23 * 8)
#define TF_S9       (24 * 8)
#define TF_S10      (25 * 8)
#define TF_S11      (26 * 8)
#define TF_T3       (27 * 8)
#define TF_T4       (28 * 8)
#define TF_T5       (29 * 8)
#define TF_T6       (30 * 8)
#define TF_SEPC     (31 * 8)
#define TF_SSTATUS  (32 * 8)
#define TF_SCAUSE   (33 * 8)
#define TF_STVAL    (34 * 8)

#define TRAPFRAME_SIZE  (35 * 8)

/* Stack size */
#define STACK_SIZE      16384

/* Kernel stack guard size */
#define STACK_GUARD     4096

#endif /* _RISCV64_SCONST_H */
