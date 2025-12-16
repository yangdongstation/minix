#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/type.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include "readclock.h"

/*
 * RISC-V 64-bit architecture readclock implementation
 *
 * RISC-V in QEMU doesn't have a traditional RTC like the i386 CMOS clock.
 * Instead, we rely on the SBI (Supervisor Binary Interface) for timer services
 * and use system time functions. The real-time clock is typically maintained
 * by the firmware/hypervisor and accessed through SBI calls or standard time
 * functions.
 */

static int rtc_initialized = 0;

/*
 * Initialize the RTC
 */
static int
riscv_rtc_init(void)
{
	rtc_initialized = 1;
	return OK;
}

/*
 * Get current time from RTC
 */
static int
riscv_rtc_get_time(struct tm *t, int flags)
{
	time_t now;
	struct tm *tm_result;

	if (!rtc_initialized) {
		return ENXIO;
	}

	/* Get current time using system time */
	if (time(&now) == (time_t)-1) {
		return errno;
	}

	/* Convert to broken-down time */
	tm_result = gmtime(&now);
	if (!tm_result) {
		return EINVAL;
	}

	/* Copy the result */
	*t = *tm_result;

	return OK;
}

/*
 * Set current time in RTC
 */
static int
riscv_rtc_set_time(struct tm *t, int flags)
{
	/* On RISC-V/QEMU, setting the RTC is typically not supported
	 * through a simple interface. The time is maintained by the
	 * hypervisor/firmware.
	 */
	return ENOSYS;
}

/*
 * Power off the system
 */
static int
riscv_rtc_pwr_off(void)
{
	/* Use SBI system reset if available */
	return ENOSYS;
}

/*
 * Cleanup RTC
 */
static void
riscv_rtc_exit(void)
{
	rtc_initialized = 0;
}

/*
 * Architecture-specific setup
 */
int
arch_setup(struct rtc *r)
{
	if (!r) {
		return EINVAL;
	}

	/* Set up the function pointers */
	r->init = riscv_rtc_init;
	r->get_time = riscv_rtc_get_time;
	r->set_time = riscv_rtc_set_time;
	r->pwr_off = riscv_rtc_pwr_off;
	r->exit = riscv_rtc_exit;

	return OK;
}