/*
 * QEMU virt platform reset support
 *
 * Uses SBI SRST extension for system reset/shutdown
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/* SBI SRST reset types */
#define SRST_TYPE_SHUTDOWN      0
#define SRST_TYPE_COLD_REBOOT   1
#define SRST_TYPE_WARM_REBOOT   2

/* SBI SRST reset reasons */
#define SRST_REASON_NONE        0
#define SRST_REASON_SYSFAIL     1

/*
 * System shutdown
 */
void bsp_shutdown(void)
{
    direct_print("System shutdown...\n");

    /* Disable interrupts */
    intr_disable();

    /* Use SBI system reset */
    sbi_system_reset(SRST_TYPE_SHUTDOWN, SRST_REASON_NONE);

    /* Should not reach here */
    for (;;) {
        wfi();
    }
}

/*
 * System reboot
 */
void bsp_reboot(void)
{
    direct_print("System reboot...\n");

    /* Disable interrupts */
    intr_disable();

    /* Use SBI system reset */
    sbi_system_reset(SRST_TYPE_COLD_REBOOT, SRST_REASON_NONE);

    /* Should not reach here */
    for (;;) {
        wfi();
    }
}

/*
 * Panic reset
 */
void bsp_panic_reset(void)
{
    /* Disable interrupts */
    intr_disable();

    /* Use SBI system reset with failure reason */
    sbi_system_reset(SRST_TYPE_SHUTDOWN, SRST_REASON_SYSFAIL);

    /* Should not reach here */
    for (;;) {
        wfi();
    }
}

/*
 * Power off
 */
void bsp_poweroff(void)
{
    bsp_shutdown();
}
