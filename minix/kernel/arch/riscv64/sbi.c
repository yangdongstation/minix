/*
 * RISC-V SBI (Supervisor Binary Interface) implementation
 *
 * Provides interface to OpenSBI/BBL firmware for:
 * - Console I/O (debugging)
 * - Timer management
 * - Inter-Processor Interrupts (IPI)
 * - System reset
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/* SBI legacy extension IDs (v0.1) */
#define SBI_EXT_0_1_SET_TIMER           0x00
#define SBI_EXT_0_1_CONSOLE_PUTCHAR     0x01
#define SBI_EXT_0_1_CONSOLE_GETCHAR     0x02
#define SBI_EXT_0_1_CLEAR_IPI           0x03
#define SBI_EXT_0_1_SEND_IPI            0x04
#define SBI_EXT_0_1_REMOTE_FENCE_I      0x05
#define SBI_EXT_0_1_REMOTE_SFENCE_VMA   0x06
#define SBI_EXT_0_1_SHUTDOWN            0x08

/* SBI v0.2+ extension IDs */
#define SBI_EXT_BASE                    0x10
#define SBI_EXT_TIME                    0x54494D45  /* "TIME" */
#define SBI_EXT_IPI                     0x735049    /* "sPI" */
#define SBI_EXT_RFENCE                  0x52464E43  /* "RFNC" */
#define SBI_EXT_HSM                     0x48534D    /* "HSM" */
#define SBI_EXT_SRST                    0x53525354  /* "SRST" */

/* SBI HSM function IDs */
#define SBI_HSM_HART_START              0
#define SBI_HSM_HART_STOP               1
#define SBI_HSM_HART_STATUS             2

/* SBI SRST function IDs */
#define SBI_SRST_RESET                  0

/* SBI reset types */
#define SBI_SRST_TYPE_SHUTDOWN          0
#define SBI_SRST_TYPE_COLD_REBOOT       1
#define SBI_SRST_TYPE_WARM_REBOOT       2

/* SBI reset reasons */
#define SBI_SRST_REASON_NONE            0
#define SBI_SRST_REASON_SYSFAIL         1

/* SBI return structure */
struct sbiret {
    long error;
    long value;
};

/* SBI error codes */
#define SBI_SUCCESS                     0
#define SBI_ERR_FAILED                  -1
#define SBI_ERR_NOT_SUPPORTED           -2
#define SBI_ERR_INVALID_PARAM           -3
#define SBI_ERR_DENIED                  -4
#define SBI_ERR_INVALID_ADDRESS         -5
#define SBI_ERR_ALREADY_AVAILABLE       -6

static unsigned long sbi_hart_mask_pa(const unsigned long *mask)
{
    phys_bytes phys;

    phys = umap_local(NULL, 0, (vir_bytes)mask, sizeof(*mask));
    if (!phys)
        panic("sbi: umap_local hart mask failed");

    return (unsigned long)phys;
}

/*
 * SBI ecall wrapper
 * Arguments passed in a0-a5, extension ID in a7, function ID in a6
 * Returns error in a0, value in a1
 */
static struct sbiret sbi_ecall(int ext, int fid,
    unsigned long arg0, unsigned long arg1,
    unsigned long arg2, unsigned long arg3,
    unsigned long arg4, unsigned long arg5)
{
    struct sbiret ret;

    register unsigned long a0 __asm__("a0") = arg0;
    register unsigned long a1 __asm__("a1") = arg1;
    register unsigned long a2 __asm__("a2") = arg2;
    register unsigned long a3 __asm__("a3") = arg3;
    register unsigned long a4 __asm__("a4") = arg4;
    register unsigned long a5 __asm__("a5") = arg5;
    register unsigned long a6 __asm__("a6") = fid;
    register unsigned long a7 __asm__("a7") = ext;

    __asm__ __volatile__(
        "ecall"
        : "+r"(a0), "+r"(a1)
        : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
        : "memory"
    );

    ret.error = a0;
    ret.value = a1;
    return ret;
}

/*
 * Console output (for debugging)
 * Uses legacy SBI v0.1 interface
 */
void sbi_console_putchar(int ch)
{
    sbi_ecall(SBI_EXT_0_1_CONSOLE_PUTCHAR, 0, ch, 0, 0, 0, 0, 0);
}

/*
 * Console input (for debugging)
 * Returns character or -1 if none available
 */
int sbi_console_getchar(void)
{
    struct sbiret ret;
    ret = sbi_ecall(SBI_EXT_0_1_CONSOLE_GETCHAR, 0, 0, 0, 0, 0, 0, 0);
    return (int)ret.error;
}

/*
 * Set timer
 * Programs the timer to fire at the specified time value
 */
void sbi_set_timer(u64_t stime_value)
{
    sbi_ecall(SBI_EXT_0_1_SET_TIMER, 0, stime_value, 0, 0, 0, 0, 0);
}

/*
 * Send IPI (Inter-Processor Interrupt) to specified harts
 * hart_mask is a bitmask of target harts
 */
void sbi_send_ipi(unsigned long hart_mask)
{
    unsigned long mask = hart_mask;

    sbi_ecall(SBI_EXT_0_1_SEND_IPI, 0, sbi_hart_mask_pa(&mask),
        0, 0, 0, 0, 0);
}

/*
 * Remote instruction cache flush
 */
void sbi_remote_fence_i(unsigned long hart_mask)
{
    unsigned long mask = hart_mask;

    sbi_ecall(SBI_EXT_0_1_REMOTE_FENCE_I, 0, sbi_hart_mask_pa(&mask),
        0, 0, 0, 0, 0);
}

/*
 * Remote TLB flush (SFENCE.VMA)
 */
void sbi_remote_sfence_vma(unsigned long hart_mask,
    unsigned long start, unsigned long size)
{
    unsigned long mask = hart_mask;

    sbi_ecall(SBI_EXT_0_1_REMOTE_SFENCE_VMA, 0,
              sbi_hart_mask_pa(&mask), start, size, 0, 0, 0);
}

/*
 * Shutdown system (legacy SBI)
 */
void sbi_shutdown(void)
{
    sbi_ecall(SBI_EXT_0_1_SHUTDOWN, 0, 0, 0, 0, 0, 0, 0);
}

/*
 * System reset using SBI v0.2+ SRST extension
 *
 * reset_type:
 *   0 = shutdown
 *   1 = cold reboot
 *   2 = warm reboot
 *
 * reset_reason:
 *   0 = none
 *   1 = system failure
 *
 * This is the key new code (~20 lines) for system reset
 */
void sbi_system_reset(u32_t reset_type, u32_t reset_reason)
{
    struct sbiret ret;

    /* Try SRST extension (SBI v0.2+) */
    ret = sbi_ecall(SBI_EXT_SRST, SBI_SRST_RESET,
                    reset_type, reset_reason, 0, 0, 0, 0);

    /* If SRST not supported, fall back to legacy shutdown */
    if (ret.error == SBI_ERR_NOT_SUPPORTED) {
        if (reset_type == SBI_SRST_TYPE_SHUTDOWN) {
            sbi_shutdown();
        } else {
            /* Can't reboot without SRST, just shutdown */
            sbi_shutdown();
        }
    }

    /* Should not reach here */
    for (;;) {
        wfi();
    }
}

#ifdef CONFIG_SMP
/*
 * Start a hart using HSM extension
 * Used for SMP boot
 */
int sbi_hsm_hart_start(unsigned long hartid,
    unsigned long start_addr, unsigned long opaque)
{
    struct sbiret ret;
    ret = sbi_ecall(SBI_EXT_HSM, SBI_HSM_HART_START,
                    hartid, start_addr, opaque, 0, 0, 0);
    return ret.error;
}

/*
 * Stop current hart
 */
int sbi_hsm_hart_stop(void)
{
    struct sbiret ret;
    ret = sbi_ecall(SBI_EXT_HSM, SBI_HSM_HART_STOP, 0, 0, 0, 0, 0, 0);
    return ret.error;
}

/*
 * Get hart status
 * Returns: 0=started, 1=stopped, 2=start_pending, 3=stop_pending
 */
int sbi_hsm_hart_status(unsigned long hartid)
{
    struct sbiret ret;
    ret = sbi_ecall(SBI_EXT_HSM, SBI_HSM_HART_STATUS, hartid, 0, 0, 0, 0, 0);
    if (ret.error != SBI_SUCCESS)
        return -1;
    return ret.value;
}
#endif /* CONFIG_SMP */
