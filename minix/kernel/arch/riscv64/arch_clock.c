/*
 * RISC-V 64 clock/timer implementation
 *
 * Uses RISC-V timer CSRs and SBI set_timer for scheduling
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"
#include "arch_clock.h"
#include "kernel/clock.h"

/* Timer frequency (from device tree, default 10 MHz for QEMU) */
static u64_t timer_freq = 10000000;

/* Ticks per clock interrupt */
static u64_t ticks_per_interrupt;

/* Next timer deadline */
static u64_t next_timer_deadline;

/* Total ticks since boot */
static volatile u64_t total_ticks;
static u64_t tsc_per_ms;

/*
 * Initialize the clock
 */
void arch_init_clock(void)
{
    /* Calculate ticks per interrupt based on configured HZ */
    ticks_per_interrupt = timer_freq / system_hz;

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

int init_local_timer(unsigned freq)
{
    (void)freq;
    arch_init_clock();
    if (timer_freq == 0)
        tsc_per_ms = 0;
    else
        tsc_per_ms = timer_freq / 1000;
    return 0;
}

void stop_local_timer(void)
{
    arch_stop_clock();
}

void restart_local_timer(void)
{
    arch_init_clock();
}

int register_local_timer_handler(const irq_handler_t handler)
{
    (void)handler;
    return 0;
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
    ticks_per_interrupt = timer_freq / system_hz;
    tsc_per_ms = timer_freq / 1000;
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

void arch_timer_int_handler(void)
{
}

void cycles_accounting_init(void)
{
    u64_t tsc;

    read_tsc_64(&tsc);
    get_cpulocal_var(tsc_ctr_switch) = tsc;
    get_cpu_var(cpuid, cpu_last_tsc) = 0;
    get_cpu_var(cpuid, cpu_last_idle) = 0;
}

void context_stop(struct proc *p)
{
    u64_t tsc;
    u64_t *ctr = get_cpulocal_var_ptr(tsc_ctr_switch);

    read_tsc_64(&tsc);
    p->p_cycles += tsc - *ctr;
    *ctr = tsc;
}

void context_stop_idle(void)
{
    int was_idle = get_cpu_var(cpuid, cpu_is_idle);

    get_cpu_var(cpuid, cpu_is_idle) = 0;
    context_stop(get_cpulocal_var_ptr(idle_proc));
    if (was_idle)
        restart_local_timer();
#if SPROFILE
    if (sprofiling)
        get_cpulocal_var(idle_interrupted) = 1;
#endif
}

u64_t ms_2_cpu_time(unsigned ms)
{
    if (tsc_per_ms == 0)
        return 0;
    return tsc_per_ms * ms;
}

unsigned cpu_time_2_ms(u64_t cpu_time)
{
    if (tsc_per_ms == 0)
        return 0;
    return (unsigned)(cpu_time / tsc_per_ms);
}

short cpu_load(void)
{
	u64_t current_tsc;
	u64_t tsc_delta, idle_delta, busy;
	u64_t *last_tsc, *last_idle;
	struct proc *idle;
	short load = 0;
#ifdef CONFIG_SMP
	unsigned cpu = cpuid;
#else
	unsigned cpu = 0;
#endif

	last_tsc = get_cpu_var_ptr(cpu, cpu_last_tsc);
	last_idle = get_cpu_var_ptr(cpu, cpu_last_idle);

	idle = get_cpu_var_ptr(cpu, idle_proc);
	read_tsc_64(&current_tsc);

	if (*last_tsc != 0) {
		tsc_delta = current_tsc - *last_tsc;
		idle_delta = idle->p_cycles - *last_idle;
		if (tsc_delta != 0) {
			busy = (tsc_delta - idle_delta) * 100;
			load = (short)(busy / tsc_delta);
			if (load > 100)
				load = 100;
		}
	}

	*last_tsc = current_tsc;
	*last_idle = idle->p_cycles;
	return load;
}

void get_cpu_ticks(unsigned int cpu, uint64_t ticks[MINIX_CPUSTATES])
{
	unsigned int i;

	(void)cpu;
	for (i = 0; i < MINIX_CPUSTATES; i++)
		ticks[i] = 0;
}

void busy_delay_ms(int ms)
{
	if (ms <= 0)
		return;
	arch_delay_us((u64_t)ms * 1000);
}
