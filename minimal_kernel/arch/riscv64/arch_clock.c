/*
 * RISC-V 64 clock/timer implementation
 *
 * Uses RISC-V timer CSRs and SBI set_timer for scheduling
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"
#include "arch_clock.h"

/* Timer frequency (from device tree, default 10 MHz for QEMU) */
static u64_t timer_freq = 10000000;

/* Ticks per clock interrupt */
static u64_t ticks_per_interrupt;

/* Next timer deadline */
static u64_t next_timer_deadline;

/* Total ticks since boot */
static volatile u64_t total_ticks;

/*
 * Initialize the clock
 */
void arch_init_clock(void)
{
    /* Calculate ticks per interrupt (assuming HZ from kernel config) */
    ticks_per_interrupt = timer_freq / HZ;

    /* Set initial timer */
    next_timer_deadline = csr_read_time() + ticks_per_interrupt;
    sbi_set_timer(next_timer_deadline);

    /* Enable timer interrupt */
    csr_set_sie(SIE_STIE);
}

/*
 * Stop the clock
 */
void arch_stop_clock(void)
{
    /* Disable timer interrupt */
    csr_clear_sie(SIE_STIE);
}

/*
 * Clock interrupt handler
 * Returns 1 if timer expired, 0 otherwise
 */
int arch_clock_handler(void)
{
    u64_t now = csr_read_time();

    /* Clear timer interrupt by setting next deadline */
    next_timer_deadline += ticks_per_interrupt;

    /* Handle wraparound or missed ticks */
    if (next_timer_deadline <= now) {
        next_timer_deadline = now + ticks_per_interrupt;
    }

    sbi_set_timer(next_timer_deadline);

    /* Increment tick counter */
    total_ticks++;

    /* Call kernel clock handler */
    /* TODO: clock_handler() from MINIX kernel */

    return 1;
}

/*
 * Get current timestamp (in timer ticks)
 */
u64_t arch_get_timestamp(void)
{
    return csr_read_time();
}

/*
 * Get total ticks since boot
 */
u64_t arch_get_ticks(void)
{
    return total_ticks;
}

/*
 * Delay for specified microseconds (busy wait)
 */
void arch_delay_us(u64_t us)
{
    u64_t start = csr_read_time();
    u64_t ticks = (us * timer_freq) / 1000000;

    while ((csr_read_time() - start) < ticks) {
        /* Busy wait */
    }
}

/*
 * Set timer frequency (called from device tree parsing)
 */
void arch_set_timer_freq(u64_t freq)
{
    timer_freq = freq;
    ticks_per_interrupt = timer_freq / HZ;
}

/*
 * Read TSC-like counter (for profiling)
 */
void read_tsc_64(u64_t *t)
{
    *t = csr_read_cycle();
}

/*
 * Get CPU flags (interrupt state)
 */
u32_t read_cpu_flags(void)
{
    return (u32_t)csr_read_sstatus();
}

/*
 * Write CPU flags
 */
void write_cpu_flags(u32_t flags)
{
    csr_write_sstatus((u64_t)flags);
}
