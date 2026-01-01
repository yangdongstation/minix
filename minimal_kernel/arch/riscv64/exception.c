/*
 * RISC-V 64 exception and interrupt handling
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

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

/* Trapframe structure (matches head.S) */
struct trapframe {
    u64_t ra;       /* x1 */
    u64_t sp;       /* x2 */
    u64_t gp;       /* x3 */
    u64_t tp;       /* x4 */
    u64_t t0;       /* x5 */
    u64_t t1;       /* x6 */
    u64_t t2;       /* x7 */
    u64_t s0;       /* x8 */
    u64_t s1;       /* x9 */
    u64_t a0;       /* x10 */
    u64_t a1;       /* x11 */
    u64_t a2;       /* x12 */
    u64_t a3;       /* x13 */
    u64_t a4;       /* x14 */
    u64_t a5;       /* x15 */
    u64_t a6;       /* x16 */
    u64_t a7;       /* x17 */
    u64_t s2;       /* x18 */
    u64_t s3;       /* x19 */
    u64_t s4;       /* x20 */
    u64_t s5;       /* x21 */
    u64_t s6;       /* x22 */
    u64_t s7;       /* x23 */
    u64_t s8;       /* x24 */
    u64_t s9;       /* x25 */
    u64_t s10;      /* x26 */
    u64_t s11;      /* x27 */
    u64_t t3;       /* x28 */
    u64_t t4;       /* x29 */
    u64_t t5;       /* x30 */
    u64_t t6;       /* x31 */
    u64_t sepc;
    u64_t sstatus;
    u64_t scause;
    u64_t stval;
};

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
    u64_t cause = tf->scause;
    int is_interrupt = (cause >> 63) & 1;

    cause &= ~(1UL << 63);  /* Clear interrupt bit */

    if (is_interrupt) {
        handle_interrupt(tf, cause);
    } else {
        handle_exception(tf, cause);
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
        handle_page_fault(tf, cause, tf->stval);
        break;

    case EXC_ILLEGAL_INST:
        /* Illegal instruction */
        panic("Illegal instruction at %p: %p", (void *)tf->sepc, (void *)tf->stval);
        break;

    case EXC_BREAKPOINT:
        /* Breakpoint - skip instruction */
        tf->sepc += 2;  /* Compressed instruction */
        break;

    case EXC_INST_MISALIGNED:
    case EXC_LOAD_MISALIGNED:
    case EXC_STORE_MISALIGNED:
        /* Misaligned access */
        panic("Misaligned access at %p: addr %p", (void *)tf->sepc, (void *)tf->stval);
        break;

    case EXC_INST_ACCESS:
    case EXC_LOAD_ACCESS:
    case EXC_STORE_ACCESS:
        /* Access fault */
        panic("Access fault at %p: addr %p", (void *)tf->sepc, (void *)tf->stval);
        break;

    default:
        panic("Unhandled exception %lu at %p", cause, (void *)tf->sepc);
    }
}

/*
 * Handle system calls
 */
static void handle_syscall(struct trapframe *tf)
{
    /* Skip ecall instruction */
    tf->sepc += 4;

    /* System call number in a7, arguments in a0-a5 */
    /* Return value goes in a0 */

    /* TODO: Route to MINIX IPC system */
    /* For now, just return error */
    tf->a0 = (u64_t)-1;
}

/*
 * Handle page faults
 */
static void handle_page_fault(struct trapframe *tf, u64_t cause, u64_t addr)
{
    int write_fault = (cause == EXC_STORE_PAGE_FAULT);
    int exec_fault = (cause == EXC_INST_PAGE_FAULT);

    /* Check if we're in kernel mode */
    if (tf->sstatus & SSTATUS_SPP) {
        /* Kernel page fault - this is bad */
        panic("Kernel page fault at %p: addr %p (cause %lu)",
              (void *)tf->sepc, (void *)addr, cause);
    }

    /* User page fault - send signal or handle COW */
    /* TODO: Integrate with MINIX VM server */
    panic("User page fault at %p: addr %p (write=%d, exec=%d)",
          (void *)tf->sepc, (void *)addr, write_fault, exec_fault);
}

/*
 * Panic - print message and halt
 */
void panic(const char *fmt, ...)
{
    /* Disable interrupts */
    intr_disable();

    /* Print panic message via SBI */
    direct_print("\n*** KERNEL PANIC ***\n");
    direct_print(fmt);
    direct_print("\n");

    /* Halt all harts */
    for (;;) {
        wfi();
    }
}
