/*
 * QEMU virt timer support
 *
 * RISC-V uses SBI for timer management.
 * The timer counter is accessible via the 'time' CSR.
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/* Timer frequency (set from device tree or default) */
static u64_t timer_freq = 10000000;  /* 10 MHz default for QEMU */

/* Ticks per HZ (scheduling tick) */
static u64_t ticks_per_hz;

/*
 * Initialize BSP timer
 */
void bsp_timer_init(u64_t freq)
{
    timer_freq = freq;
    ticks_per_hz = timer_freq / system_hz;

    /* Set first timer interrupt */
    u64_t now = csr_read_time();
    sbi_set_timer(now + ticks_per_hz);

    /* Enable timer interrupt */
    csr_set_sie(SIE_STIE);
}

/*
 * Get timer frequency
 */
u64_t bsp_timer_get_freq(void)
{
    return timer_freq;
}

/*
 * Read current timer value
 */
u64_t bsp_timer_read(void)
{
    return csr_read_time();
}

/*
 * Set next timer deadline
 */
void bsp_timer_set_next(u64_t deadline)
{
    sbi_set_timer(deadline);
}

/*
 * Timer interrupt handler
 */
void bsp_timer_intr(void)
{
    /* Schedule next timer interrupt */
    u64_t now = csr_read_time();
    sbi_set_timer(now + ticks_per_hz);

    /* Call kernel clock handler */
    /* TODO: clock_handler() */
}

/*
 * Delay for specified microseconds
 */
void bsp_timer_delay_us(u64_t us)
{
    u64_t start = csr_read_time();
    u64_t ticks = (us * timer_freq) / 1000000;

    while ((csr_read_time() - start) < ticks) {
        /* Busy wait */
    }
}

/*
 * Delay for specified milliseconds
 */
void bsp_timer_delay_ms(u64_t ms)
{
    bsp_timer_delay_us(ms * 1000);
}
