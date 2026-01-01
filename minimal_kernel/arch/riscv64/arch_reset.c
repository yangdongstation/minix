/*
 * RISC-V 64 system reset implementation
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/*
 * Reset the system
 */
void arch_reset(void)
{
    direct_print("System reset...\n");

    /* Disable interrupts */
    intr_disable();

    /* Try to reboot via SBI */
    sbi_system_reset(1, 0);  /* Cold reboot */

    /* If SBI reboot failed, try shutdown */
    sbi_shutdown();

    /* Should never reach here */
    for (;;) {
        wfi();
    }
}

/*
 * Halt the system
 */
void arch_halt(void)
{
    direct_print("System halted.\n");

    /* Disable interrupts */
    intr_disable();

    /* Halt all harts */
    for (;;) {
        wfi();
    }
}

/*
 * Power off the system
 */
void arch_poweroff(void)
{
    direct_print("Power off.\n");

    /* Disable interrupts */
    intr_disable();

    /* Shutdown via SBI */
    sbi_system_reset(0, 0);  /* Shutdown */

    /* Fallback */
    sbi_shutdown();

    /* Should never reach here */
    for (;;) {
        wfi();
    }
}
