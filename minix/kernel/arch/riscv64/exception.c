/*
 * RISC-V 64 exception and interrupt handling
 */

#include <string.h>
#include <signal.h>

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

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
void copr_not_available_handler(void);
extern int catch_pagefaults;

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

    if (from_user) {
        caller = get_cpulocal_var(proc_ptr);
        if (caller != NULL) {
            memcpy(&caller->p_reg, (struct stackframe_s *)tf,
                sizeof(caller->p_reg));
        }
    }

    cause &= ~(1UL << 63);  /* Clear interrupt bit */

    if (is_interrupt) {
        handle_interrupt(tf, cause);
    } else {
        handle_exception(tf, cause);
    }

    if (from_user && caller != NULL) {
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
    struct proc *pr = get_cpulocal_var(proc_ptr);
    int from_user = (tf->tf_sstatus & SSTATUS_SPP) == 0;

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
        /* Illegal instruction (may be FPU use with FS=OFF) */
        if (from_user &&
            ((tf->tf_sstatus & SSTATUS_FS_MASK) == SSTATUS_FS_OFF)) {
            copr_not_available_handler();
            NOT_REACHABLE;
        }
        if (from_user && pr != NULL) {
            cause_sig(proc_nr(pr), SIGILL);
            return;
        }
        panic("Illegal instruction at %p: %p", (void *)tf->tf_sepc,
            (void *)tf->tf_stval);
        break;

    case EXC_BREAKPOINT:
        if (from_user && pr != NULL) {
            cause_sig(proc_nr(pr), SIGEMT);
            return;
        }
        /* Breakpoint in kernel - skip instruction */
        tf->tf_sepc += 4;
        break;

    case EXC_INST_MISALIGNED:
    case EXC_LOAD_MISALIGNED:
    case EXC_STORE_MISALIGNED:
        /* Misaligned access */
        if (from_user && pr != NULL) {
            cause_sig(proc_nr(pr), SIGBUS);
            return;
        }
        panic("Misaligned access at %p: addr %p", (void *)tf->tf_sepc,
            (void *)tf->tf_stval);
        break;

    case EXC_INST_ACCESS:
    case EXC_LOAD_ACCESS:
    case EXC_STORE_ACCESS:
        /* Access fault */
        if (from_user && pr != NULL) {
            cause_sig(proc_nr(pr), SIGSEGV);
            return;
        }
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

    if (caller == NULL) {
        tf->tf_a0 = (u64_t)-1;
        return;
    }

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
    int in_physcopy = 0;
    int err;
    message m_pagefault;

    in_physcopy = (tf->tf_sepc > (vir_bytes)phys_copy) &&
        (tf->tf_sepc < (vir_bytes)phys_copy_fault);

    if (catch_pagefaults && in_physcopy) {
        if (tf->tf_sstatus & SSTATUS_SPP) {
            tf->tf_sepc = (u64_t)phys_copy_fault_in_kernel;
        } else if (pr != NULL) {
            pr->p_reg.pc = (reg_t)phys_copy_fault;
        }
        return;
    }

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
