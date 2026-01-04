/*
 * RISC-V 64 VM control operations
 *
 * Handles VMCTL kernel calls for memory management
 */

#include "kernel/kernel.h"
#include "kernel/system.h"
#include "archconst.h"
#include "arch_proto.h"

static phys_bytes get_current_pgdir(void);
static void set_satp(struct proc *p, phys_bytes root, reg_t *root_v);

static void set_satp(struct proc *p, phys_bytes root, reg_t *root_v)
{
    static int switch_trace_count;

    p->p_seg.p_satp = root;
    /* Prefer kernel-mapped root; fall back to identity mapping. */
    if (root_v != NULL)
        p->p_seg.p_satp_v = root_v;
    else
        p->p_seg.p_satp_v = (reg_t *)root;

    if (p == get_cpulocal_var(ptproc)) {
        if (switch_trace_count < 4)
            direct_print("rv64: setaddrspace switch\n");
        if (switch_trace_count < 4) {
            direct_print("rv64: setaddrspace fn=");
            direct_print_hex((u64_t)(uintptr_t)set_satp);
            direct_print(" pgdir_fn=");
            direct_print_hex((u64_t)(uintptr_t)set_pgdir);
            direct_print("\n");
        }
        if (p->p_endpoint == VM_PROC_NR)
            csr_set_sstatus(SSTATUS_SUM);
        else
            csr_clear_sstatus(SSTATUS_SUM);
        set_pgdir(root);
        if (switch_trace_count < 4)
            direct_print("rv64: setaddrspace switched\n");
        pg_flush_tlb();
        if (switch_trace_count < 4) {
            direct_print("rv64: setaddrspace tlb\n");
            switch_trace_count++;
        }
    }

    RTS_UNSET(p, RTS_VMINHIBIT);
}

/*
 * Architecture-specific VM control
 */
int arch_do_vmctl(message *m_ptr, struct proc *p)
{
    int request = m_ptr->SVMCTL_PARAM;
    static int vmctl_trace_count;

    switch (request) {
    case VMCTL_GET_PDBR:
        /* Return page directory base register (SATP) */
        if (p->p_seg.p_satp != 0)
            m_ptr->SVMCTL_VALUE = (u32_t)p->p_seg.p_satp;
        else
            m_ptr->SVMCTL_VALUE = (u32_t)get_current_pgdir();
        return OK;

    case VMCTL_FLUSHTLB:
        /* Flush TLB */
        pg_flush_tlb();
        return OK;

    case VMCTL_SETADDRSPACE:
        /* Set address space for process */
        {
            phys_bytes root = (phys_bytes)(u32_t)m_ptr->SVMCTL_PTROOT;
            if (vmctl_trace_count < 8) {
                direct_print("rv64: vmctl setaddrspace ep=");
                direct_print_hex((u64_t)p->p_endpoint);
                direct_print(" root=");
                direct_print_hex((u64_t)root);
                direct_print(" root_v=");
                direct_print_hex((u64_t)m_ptr->SVMCTL_PTROOT_V);
                direct_print("\n");
                vmctl_trace_count++;
            }
            set_satp(p, root, (reg_t *)m_ptr->SVMCTL_PTROOT_V);
            return OK;
        }

    default:
        return EINVAL;
    }
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
