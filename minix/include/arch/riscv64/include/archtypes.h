/*
 * RISC-V 64 architecture types for MINIX
 */

#ifndef _RISCV64_ARCHTYPES_H
#define _RISCV64_ARCHTYPES_H

#include <sys/types.h>

/* Register types */
typedef unsigned long reg_t;        /* Machine register */
typedef unsigned long segdesc_t;    /* Segment descriptor (not used on RISC-V) */

/* Kernel information structure */
struct kinfo {
    phys_bytes code_base;       /* Kernel code start */
    phys_bytes code_size;       /* Kernel code size */
    phys_bytes data_base;       /* Kernel data start */
    phys_bytes data_size;       /* Kernel data size */
    vir_bytes  proc_addr;       /* Process table virtual address */
    phys_bytes kmem_base;       /* Kernel memory start */
    phys_bytes kmem_size;       /* Kernel memory size */
    phys_bytes bootmod_base;    /* Boot modules start */
    phys_bytes bootmod_size;    /* Boot modules size */
};

/* Machine context for signals/coroutines */
struct mcontext {
    reg_t mc_gregs[32];         /* General purpose registers */
    reg_t mc_pc;                /* Program counter */
    reg_t mc_status;            /* Status register */
    double mc_fpregs[32];       /* Floating point registers */
    reg_t mc_fcsr;              /* FP control/status register */
};

/* Segment descriptor (stub for compatibility) */
struct segdesc_s {
    unsigned long base;
    unsigned long limit;
    unsigned long flags;
};

#endif /* _RISCV64_ARCHTYPES_H */
