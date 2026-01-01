/* getidle.c - by David van Moolenbroek <dcvmoole@cs.vu.nl> */

/*
 * RISC-V 64 idle time measurement.
 */

#define _SYSTEM		1
#include <lib.h>
#include <minix/u64.h>
#include <minix/sysutil.h>
#include <minix/syslib.h>

static u64_t start, idle;
static int running = 0;

static double make_double(u64_t d)
{
/* Convert a 64-bit fixed point value into a double.
 * This whole thing should be replaced by something better eventually.
 */
	double value;
	size_t i;

	value = (double) ex64hi(d);
	for (i = 0; i < sizeof(unsigned long); i += 2)
		value *= 65536.0;

	value += (double) ex64lo(d);

	return value;
}

static inline u64_t read_cycle_64(void)
{
	u64_t val;

	__asm__ __volatile__("rdcycle %0" : "=r"(val));
	return val;
}

double getidle(void)
{
	u64_t stop, idle2;
	u64_t idelta, tdelta;
	double ifp, tfp, rfp;
	int r;

	if (!running) {
		if ((r = sys_getidletsc(&idle)) != OK)
			return -1.0;

		running = 1;

		start = read_cycle_64();

		return 0.0;
	} else {
		stop = read_cycle_64();

		running = 0;

		if ((r = sys_getidletsc(&idle2)) != OK)
			return -1.0;

		idelta = idle2 - idle;
		tdelta = stop - start;

		if (idelta >= tdelta)
			return 100.0;

		ifp = make_double(idelta);
		tfp = make_double(tdelta);

		rfp = ifp / tfp * 100.0;

		if (rfp < 0.0) rfp = 0.0;
		else if (rfp > 100.0) rfp = 100.0;

		return rfp;
	}
}
