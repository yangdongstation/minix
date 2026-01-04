/*
 * RISC-V 64 system management functions
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/* Shutdown types */
#define RBT_HALT        0
#define RBT_REBOOT      1
#define RBT_PANIC       2
#define RBT_POWEROFF    3

/*
 * Initialize architecture-specific system components
 */
void arch_system_init(void)
{
    /* Initialize exception handling */
    exception_init();

    /* Initialize hardware interrupts (PLIC) */
    hw_intr_init();

    /* Initialize clock */
    arch_init_clock();

    /* TODO: Initialize other subsystems */
}

/*
 * System shutdown/reboot
 */
void arch_shutdown(int how)
{
    /* Disable interrupts */
    intr_disable();

    switch (how) {
    case RBT_HALT:
    case RBT_PANIC:
        direct_print("System halted.\n");
        sbi_system_reset(0, 0);  /* Shutdown */
        break;

    case RBT_REBOOT:
        direct_print("Rebooting...\n");
        sbi_system_reset(1, 0);  /* Cold reboot */
        break;

    case RBT_POWEROFF:
        direct_print("Power off.\n");
        sbi_system_reset(0, 0);  /* Shutdown */
        break;

    default:
        direct_print("Unknown shutdown type, halting.\n");
        sbi_system_reset(0, 0);
    }

    /* Should not reach here */
    for (;;) {
        wfi();
    }
}

/*
 * Return to monitor (not applicable on RISC-V)
 */
void arch_monitor(void)
{
    /* No monitor on RISC-V, just halt */
    arch_shutdown(RBT_HALT);
}

/*
 * Get a.out headers (for binary loading)
 */
void arch_get_aout_headers(int i, struct exec *h)
{
    /* TODO: Implement if needed for a.out binary support */
    (void)i;
    (void)h;
}

void arch_set_secondary_ipc_return(struct proc *p, reg_t val)
{
    p->p_reg.a1 = val;
}

/*
 * Architecture-specific reset
 */
void arch_reset(void)
{
    arch_shutdown(RBT_REBOOT);
}

/*
 * Idle loop
 */
void idle(void)
{
    /* Enable interrupts and wait */
    intr_enable();
    wfi();
}

/*
 * Execute function at privilege level 0 (kernel)
 */
void level0(void (*func)(void))
{
    /* Already in S-mode, just call the function */
    func();
}
