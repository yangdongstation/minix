/* Helper functions that allow driver writers to easily busy-wait (spin) for a
 * condition to become satisfied within a certain maximum time span.
 */
#include "sysutil.h"
#include <minix/spin.h>

/* Number of microseconds to keep spinning initially, without performing a
 * system call. We pick a value somewhat smaller than a typical clock tick.
 */
#define TSC_SPIN		1000		/* in microseconds */

/* Internal spin states. */
enum {
	STATE_INIT,		/* simply check the condition (once) */
	STATE_BASE_TS,		/* get the initial timebase value (once) */
	STATE_TS,		/* use timebase to spin (up to TSC_SPIN us) */
	STATE_UPTIME		/* use the clock to spin */
};

/* Timer frequency (default for QEMU). */
static u64_t timer_freq = 10000000;

static inline u64_t read_time(void)
{
	u64_t val;

	__asm__ __volatile__("rdtime %0" : "=r"(val));
	return val;
}

static u32_t ticks_to_micros(u64_t ticks)
{
	return (u32_t)((ticks * 1000000) / timer_freq);
}

void spin_init(spin_t *s, u32_t usecs)
{
	/* Initialize the given spin state structure, set to spin at most the
	 * given number of microseconds.
	 */
	s->s_state = STATE_INIT;
	s->s_usecs = usecs;
	s->s_timeout = FALSE;
}

int spin_check(spin_t *s)
{
	/* Check whether a timeout has taken place. Return TRUE if the caller
	 * should continue spinning, and FALSE if a timeout has occurred. The
	 * implementation assumes that it is okay to spin a little bit too long
	 * (up to a full clock tick extra).
	 */
	u64_t cur_tsc, tsc_delta;
	clock_t now, micro_delta;

	switch (s->s_state) {
	case STATE_INIT:
		s->s_state = STATE_BASE_TS;
		break;

	case STATE_BASE_TS:
		s->s_state = STATE_TS;
		s->s_base_tsc = read_time();
		break;

	case STATE_TS:
		cur_tsc = read_time();
		tsc_delta = cur_tsc - s->s_base_tsc;
		micro_delta = ticks_to_micros(tsc_delta);

		if (micro_delta >= s->s_usecs) {
			s->s_timeout = TRUE;
			return FALSE;
		}

		if (micro_delta >= TSC_SPIN) {
			s->s_usecs -= micro_delta;
			s->s_base_uptime = getticks();
			s->s_state = STATE_UPTIME;
		}

		break;

	case STATE_UPTIME:
		now = getticks();

		/* We assume that sys_hz() caches its return value. */
		micro_delta = ((now - s->s_base_uptime) * 1000 / sys_hz()) *
			1000;

		if (micro_delta >= s->s_usecs) {
			s->s_timeout = TRUE;
			return FALSE;
		}

		break;

	default:
		panic("spin_check: invalid state %d", s->s_state);
	}

	return TRUE;
}

int micro_delay(u32_t micros)
{
	u64_t start, ticks;

	start = read_time();
	ticks = ((u64_t)micros * timer_freq) / 1000000;

	while ((read_time() - start) < ticks) {
		/* Busy wait. */
	}

	return OK;
}
