/*
 * RISC-V 64 architecture types for MINIX
 */

#ifndef _RISCV64_ARCHTYPES_H
#define _RISCV64_ARCHTYPES_H

#include <minix/sys_config.h>
#include <machine/stackframe.h>
#include <sys/cdefs.h>
#include <sys/types.h>

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

typedef struct segframe {
	reg_t	p_satp;		/* page table root */
	reg_t	*p_satp_v;
	char	*fpu_state;
} segframe_t;

struct cpu_info {
	u32_t	hartid;
	u32_t	freq;		/* in MHz */
};

typedef u32_t atomic_t;	/* access to an aligned 32bit value is atomic */

#endif /* _RISCV64_ARCHTYPES_H */
