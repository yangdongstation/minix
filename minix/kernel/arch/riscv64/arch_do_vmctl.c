/*
 * RISC-V 64 VM control operations
 *
 * Handles VMCTL kernel calls for memory management
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/* VMCTL request codes (from MINIX IPC) */
#define VMCTL_GET_PAGEFAULT     1
#define VMCTL_CLEAR_PAGEFAULT   2
#define VMCTL_SET_ADDRSPACE     3
#define VMCTL_GET_PDBR          4
#define VMCTL_FLUSHTLB          5
#define VMCTL_MEMREQ_GET        6
#define VMCTL_MEMREQ_REPLY      7

/*
 * Architecture-specific VM control
 */
int arch_do_vmctl(message *m_ptr, struct proc *p)
{
    int request = m_ptr->m_type;  /* Simplified */

    switch (request) {
    case VMCTL_GET_PDBR:
        /* Return page directory base register (SATP) */
        m_ptr->m_type = (int)(csr_read_satp() & SATP_PPN_MASK);
        break;

    case VMCTL_FLUSHTLB:
        /* Flush TLB */
        pg_flush_tlb();
        break;

    case VMCTL_SET_ADDRSPACE:
        /* Set address space for process */
        /* TODO: Update process page table */
        break;

    default:
        return -1;  /* Not supported */
    }

    return 0;
}

/*
 * Enable paging
 */
void enable_paging(void)
{
    extern u64_t _boot_pgdir[];
    u64_t satp;

    satp = ((phys_bytes)_boot_pgdir >> 12) | SATP_MODE_SV39;
    csr_write_satp(satp);
}

/*
 * Get current page directory
 */
phys_bytes get_current_pgdir(void)
{
    u64_t satp = csr_read_satp();
    return (satp & SATP_PPN_MASK) << 12;
}

/*
 * Set page directory for current CPU
 */
void set_pgdir(phys_bytes pgdir)
{
    u64_t satp = (pgdir >> 12) | SATP_MODE_SV39;
    csr_write_satp(satp);
}
