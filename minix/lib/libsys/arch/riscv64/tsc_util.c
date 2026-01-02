/*
 * RISC-V 64 timestamp counter utilities
 */

#include <lib.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/u64.h>

/* Timer frequency */
static u64_t tsc_freq = 10000000;  /* Default 10 MHz for QEMU */

/*
 * Initialize TSC utilities
 */
void tsc_init(void)
{
    /* TODO: Get actual frequency from kernel/device tree */
    tsc_freq = 10000000;
}

/*
 * Read cycle counter
 */
void read_tsc(u32_t *hi, u32_t *lo)
{
    u64_t val;
    __asm__ __volatile__("rdcycle %0" : "=r"(val));
    *lo = (u32_t)val;
    *hi = (u32_t)(val >> 32);
}

/*
 * Read 64-bit cycle counter
 */
void read_tsc_64(u64_t *t)
{
    u64_t val;
    __asm__ __volatile__("rdcycle %0" : "=r"(val));
    *t = val;
}

/*
 * Get TSC frequency in Hz
 */
u64_t tsc_get_freq(void)
{
    return tsc_freq;
}

/*
 * Convert TSC ticks to microseconds
 */
u64_t tsc_to_usec(u64_t ticks)
{
    return (ticks * 1000000) / tsc_freq;
}

/*
 * Convert microseconds to TSC ticks
 */
u64_t usec_to_tsc(u64_t usec)
{
    return (usec * tsc_freq) / 1000000;
}

u32_t tsc_64_to_micros(u64_t tsc)
{
    return (u32_t)tsc_to_usec(tsc);
}

u32_t tsc_to_micros(u32_t low, u32_t high)
{
    return tsc_64_to_micros(make64(low, high));
}

u32_t tsc_get_khz(void)
{
    return (u32_t)(tsc_freq / 1000);
}
