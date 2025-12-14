/*
 * RISC-V 64 spin/delay functions
 */

#include <lib.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>

/* Timer frequency (default for QEMU) */
static u64_t timer_freq = 10000000;

/*
 * Read time counter
 */
static inline u64_t read_time(void)
{
    u64_t val;
    __asm__ __volatile__("rdtime %0" : "=r"(val));
    return val;
}

/*
 * Spin for specified number of microseconds
 */
void micro_delay(u32_t usecs)
{
    u64_t start = read_time();
    u64_t ticks = ((u64_t)usecs * timer_freq) / 1000000;

    while ((read_time() - start) < ticks) {
        /* Busy wait */
    }
}

/*
 * Spin for specified number of milliseconds
 */
void milli_delay(u32_t msecs)
{
    micro_delay(msecs * 1000);
}

/*
 * Initialize spin timing (get actual timer frequency)
 */
void spin_init(void)
{
    /* TODO: Get actual frequency from device tree */
    timer_freq = 10000000;  /* 10 MHz default for QEMU */
}

/*
 * Read cycle counter for timing
 */
u64_t read_tsc_64(void)
{
    u64_t val;
    __asm__ __volatile__("rdcycle %0" : "=r"(val));
    return val;
}

/*
 * Get timestamp in microseconds
 */
u64_t get_time_usec(void)
{
    return (read_time() * 1000000) / timer_freq;
}
