/*
 * RISC-V 64 protection/privilege management
 *
 * RISC-V uses privilege levels:
 * - M-mode (Machine): Highest privilege, runs OpenSBI
 * - S-mode (Supervisor): Kernel runs here
 * - U-mode (User): User processes
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/*
 * Initialize protection
 */
void prot_init(void)
{
    /* On RISC-V, we're already in S-mode after OpenSBI handoff */

    /* Configure sstatus for default state */
    u64_t sstatus = csr_read_sstatus();

    /* Clear SPP (will return to U-mode after sret) */
    sstatus &= ~SSTATUS_SPP;

    /* Enable FPU if present */
    sstatus |= SSTATUS_FS_INITIAL;

    /* Disable SUM (supervisor can't access user memory by default) */
    sstatus &= ~SSTATUS_SUM;

    csr_write_sstatus(sstatus);

    /* Set up PMP if needed (usually done by OpenSBI) */
}

/*
 * Set kernel segment limit (not applicable on RISC-V)
 */
void prot_set_kern_seg_limit(vir_bytes limit)
{
    /* RISC-V uses page tables, not segments */
    (void)limit;
}

/*
 * Check if address is kernel address
 */
int is_kernel_addr(vir_bytes addr)
{
    /* Kernel addresses are in high memory (negative in 2's complement) */
    return (addr >= KERNEL_VIRT_BASE);
}

/*
 * Check if current mode is kernel
 */
int in_kernel_mode(void)
{
    /* Check SPP bit in sstatus */
    return (csr_read_sstatus() & SSTATUS_SPP) != 0;
}

/*
 * Enable user memory access from supervisor
 */
void enable_user_access(void)
{
    csr_set_sstatus(SSTATUS_SUM);
}

/*
 * Disable user memory access from supervisor
 */
void disable_user_access(void)
{
    csr_clear_sstatus(SSTATUS_SUM);
}

/*
 * Save FPU state
 */
void save_fpu(struct proc *p)
{
    /* TODO: Save FPU registers to proc structure */
    (void)p;
}

/*
 * Restore FPU state
 */
void restore_fpu(struct proc *p)
{
    /* TODO: Restore FPU registers from proc structure */
    (void)p;
}
