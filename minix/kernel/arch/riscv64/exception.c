/*
 * RISC-V 64 exception and interrupt handling
 */

#include <string.h>

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

static message m_pagefault;

void proc_stacktrace(struct proc *proc)
{
	if (proc == NULL)
		return;

	printf("stacktrace %s/%d pc=%p sp=%p ra=%p\n",
		proc->p_name, proc->p_endpoint,
		(void *)proc->p_reg.pc, (void *)proc->p_reg.sp,
		(void *)proc->p_reg.ra);
}

/* Exception cause codes */
#define EXC_INST_MISALIGNED     0
#define EXC_INST_ACCESS         1
#define EXC_ILLEGAL_INST        2
#define EXC_BREAKPOINT          3
#define EXC_LOAD_MISALIGNED     4
#define EXC_LOAD_ACCESS         5
#define EXC_STORE_MISALIGNED    6
#define EXC_STORE_ACCESS        7
#define EXC_ECALL_U             8
#define EXC_ECALL_S             9
#define EXC_INST_PAGE_FAULT     12
#define EXC_LOAD_PAGE_FAULT     13
#define EXC_STORE_PAGE_FAULT    15

/* Interrupt cause codes (bit 63 set) */
#define INT_S_SOFT              1
#define INT_S_TIMER             5
#define INT_S_EXTERNAL          9

/* Forward declarations */
static void handle_interrupt(struct trapframe *tf, u64_t cause);
static void handle_exception(struct trapframe *tf, u64_t cause);
static void handle_syscall(struct trapframe *tf);
static void handle_page_fault(struct trapframe *tf, u64_t cause, u64_t addr);

/*
 * Initialize exception handling
 */
void exception_init(void)
{
    extern void trap_entry(void);

    /* Set trap vector (direct mode, all traps go to trap_entry) */
    csr_write_stvec((u64_t)trap_entry);

    /* Enable timer and external interrupts */
    csr_set_sie(SIE_STIE | SIE_SEIE);
}

/*
 * Main exception handler (called from assembly)
 */
void exception_handler(struct trapframe *tf)
{
    u64_t cause = tf->tf_scause;
    int is_interrupt = (cause >> 63) & 1;
    struct proc *caller = NULL;
    int from_user = (tf->tf_sstatus & SSTATUS_SPP) == 0;
    static int trap_trace_count;

    if (from_user) {
        caller = get_cpulocal_var(proc_ptr);
        if (caller != NULL) {
            memcpy(&caller->p_reg, (struct stackframe_s *)tf,
                sizeof(caller->p_reg));
        }
    }

    if (!is_interrupt && !from_user && trap_trace_count < 128) {
        direct_print("rv64: trap scause=");
        direct_print_hex(cause);
        direct_print(" sepc=");
        direct_print_hex(tf->tf_sepc);
        direct_print(" stval=");
        direct_print_hex(tf->tf_stval);
        direct_print(" sstatus=");
        direct_print_hex(tf->tf_sstatus);
        direct_print("\n");
        trap_trace_count++;
    }

    cause &= ~(1UL << 63);  /* Clear interrupt bit */

    if (is_interrupt) {
        handle_interrupt(tf, cause);
    } else {
        handle_exception(tf, cause);
    }

    if (from_user && caller != NULL) {
#if defined(__riscv)
        {
            static int sw_log_count;
            if (sw_log_count < 32) {
                direct_print("rv64: exception switch_to_user\n");
                sw_log_count++;
            }
        }
#endif
        switch_to_user();
        NOT_REACHABLE;
    }
}

/*
 * Handle interrupts
 */
static void handle_interrupt(struct trapframe *tf, u64_t cause)
{
    switch (cause) {
    case INT_S_SOFT:
        /* Software interrupt (IPI) */
        csr_clear_sip(SIP_SSIP);
#ifdef CONFIG_SMP
        smp_ipi_handler(tf);
#endif
        break;

    case INT_S_TIMER:
        /* Timer interrupt */
        arch_clock_handler();
        break;

    case INT_S_EXTERNAL:
        /* External interrupt (PLIC) */
        hw_intr_handler(tf);
        break;

    default:
        panic("Unknown interrupt: %lu", cause);
    }
}

/*
 * Handle exceptions (synchronous traps)
 */
static void handle_exception(struct trapframe *tf, u64_t cause)
{
    switch (cause) {
    case EXC_ECALL_U:
        /* System call from user mode */
        handle_syscall(tf);
        break;

    case EXC_INST_PAGE_FAULT:
    case EXC_LOAD_PAGE_FAULT:
    case EXC_STORE_PAGE_FAULT:
        /* Page fault */
        handle_page_fault(tf, cause, tf->tf_stval);
        break;

    case EXC_ILLEGAL_INST:
        /* Illegal instruction */
        panic("Illegal instruction at %p: %p", (void *)tf->tf_sepc,
            (void *)tf->tf_stval);
        break;

    case EXC_BREAKPOINT:
        /* Breakpoint - skip instruction */
        tf->tf_sepc += 2;  /* Compressed instruction */
        break;

    case EXC_INST_MISALIGNED:
    case EXC_LOAD_MISALIGNED:
    case EXC_STORE_MISALIGNED:
        /* Misaligned access */
        panic("Misaligned access at %p: addr %p", (void *)tf->tf_sepc,
            (void *)tf->tf_stval);
        break;

    case EXC_INST_ACCESS:
    case EXC_LOAD_ACCESS:
    case EXC_STORE_ACCESS:
        /* Access fault */
        panic("Access fault at %p: addr %p", (void *)tf->tf_sepc,
            (void *)tf->tf_stval);
        break;

    default:
        panic("Unhandled exception %lu at %p", cause, (void *)tf->tf_sepc);
    }
}

/*
 * Handle system calls
 */
static void handle_syscall(struct trapframe *tf)
{
    struct proc *caller = get_cpulocal_var(proc_ptr);
    static int syscall_trace_count;

    if (caller == NULL) {
        tf->tf_a0 = (u64_t)-1;
        return;
    }

    if (syscall_trace_count < 256) {
        direct_print("rv64: syscall a7=");
        direct_print_hex(caller->p_reg.a7);
        direct_print(" a2=");
        direct_print_hex(caller->p_reg.a2);
        direct_print(" a0=");
        direct_print_hex(caller->p_reg.a0);
        direct_print(" a1=");
        direct_print_hex(caller->p_reg.a1);
        direct_print("\n");
        syscall_trace_count++;
    }
#if defined(__riscv)
    {
        static int ipc_vm_trace_count;
        if (caller->p_reg.a7 == IPCVEC_INTR &&
            caller->p_reg.a0 == VM_PROC_NR &&
            ipc_vm_trace_count < 64) {
            message m;
            int cr = EFAULT;
            int op = (int)caller->p_reg.a2;

            if ((op == SEND || op == SENDREC || op == SENDNB) &&
                caller->p_reg.a1 != 0) {
                cr = data_copy_vmcheck(caller, caller->p_endpoint,
                    (vir_bytes)caller->p_reg.a1, KERNEL,
                    (vir_bytes)&m, sizeof(m));
            }

            direct_print("rv64: ipc vm op=");
            direct_print_hex((u64_t)op);
            direct_print(" src=");
            direct_print_hex((u64_t)caller->p_endpoint);
            direct_print(" ptr=");
            direct_print_hex((u64_t)caller->p_reg.a1);
            if (op == SEND || op == SENDREC || op == SENDNB) {
                direct_print(" mtype=");
                if (cr == OK) {
                    direct_print_hex((u64_t)m.m_type);
                } else {
                    direct_print("copyfail");
                }
            }
            direct_print("\n");
            ipc_vm_trace_count++;
        }
    }
#endif

    /* Skip ecall instruction */
    caller->p_reg.pc += 4;

    switch (caller->p_reg.a7) {
    case IPCVEC_INTR:
        caller->p_reg.retreg = do_ipc(caller->p_reg.a2,
            caller->p_reg.a0, caller->p_reg.a1);
        break;
    case KERVEC_INTR:
        kernel_call((message *)caller->p_reg.a0, caller);
        break;
    default:
        caller->p_reg.retreg = EBADCALL;
        break;
    }
}

/*
 * Handle page faults
 */
static void handle_page_fault(struct trapframe *tf, u64_t cause, u64_t addr)
{
    int write_fault = (cause == EXC_STORE_PAGE_FAULT);
    int exec_fault = (cause == EXC_INST_PAGE_FAULT);
    struct proc *pr = get_cpulocal_var(proc_ptr);
    int err;

    /* Check if we're in kernel mode */
    if (tf->tf_sstatus & SSTATUS_SPP) {
        /* Kernel page fault - this is bad */
        panic("Kernel page fault at %p: addr %p (cause %lu)",
              (void *)tf->tf_sepc, (void *)addr, cause);
    }

    /* VM can't handle page faults for itself. */
    if (pr && pr->p_endpoint == VM_PROC_NR) {
        direct_print("rv64: VM pagefault pc=");
        direct_print_hex(pr->p_reg.pc);
        direct_print(" stval=");
        direct_print_hex(addr);
        direct_print(" cause=");
        direct_print_hex(cause);
        direct_print(" sstatus=");
        direct_print_hex(tf->tf_sstatus);
        direct_print(" satp=");
        direct_print_hex(csr_read_satp());
        direct_print("\n");
        printf("pagefault for VM on CPU %d, pc = 0x%lx, addr = 0x%lx, cause = 0x%lx\n",
            cpuid, pr->p_reg.pc, addr, cause);
        proc_stacktrace(pr);
        panic("pagefault in VM");
    }

    /* Don't schedule this process until pagefault is handled. */
    if (pr) {
        static int user_pf_log_count;
        int log_pf = (user_pf_log_count < 64) || (addr < 0x1000);
        if (log_pf) {
            u64_t satp = csr_read_satp();
            u64_t root = (satp & SATP_PPN_MASK) << 12;
            direct_print("rv64: user pagefault ep=");
            direct_print_hex(pr->p_endpoint);
            direct_print(" name=");
            direct_print(pr->p_name);
            direct_print(" pc=");
            direct_print_hex(pr->p_reg.pc);
            direct_print(" ra=");
            direct_print_hex(pr->p_reg.ra);
            direct_print(" sp=");
            direct_print_hex(pr->p_reg.sp);
            direct_print(" addr=");
            direct_print_hex(addr);
            direct_print(" cause=");
            direct_print_hex(cause);
            direct_print(" satp=");
            direct_print_hex(satp);
            direct_print(" root=");
            direct_print_hex(root);
            direct_print(" p_seg=");
            direct_print_hex(pr->p_seg.p_satp);
            direct_print(" p_seg_v=");
            direct_print_hex((u64_t)(uintptr_t)pr->p_seg.p_satp_v);
            direct_print("\n");
            if (user_pf_log_count < 64)
                user_pf_log_count++;
        }
        RTS_SET(pr, RTS_PAGEFAULT);

        /* tell VM about the pagefault */
        m_pagefault.m_source = pr->p_endpoint;
        m_pagefault.m_type   = VM_PAGEFAULT;
        m_pagefault.VPF_ADDR = addr;
        m_pagefault.VPF_FLAGS = (u32_t)cause;

        if ((err = mini_send(pr, VM_PROC_NR,
                &m_pagefault, FROM_KERNEL))) {
            panic("WARNING: pagefault: mini_send returned %d\n", err);
        }
        return;
    }

    /* Fallback if we don't have a process pointer. */
    panic("User page fault at %p: addr %p (write=%d, exec=%d)",
          (void *)tf->tf_sepc, (void *)addr, write_fault, exec_fault);
}
