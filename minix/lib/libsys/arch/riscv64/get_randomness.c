/*
 * RISC-V 64 random number generation.
 *
 * Uses the cycle counter as an entropy source.
 */

#include <lib.h>
#include <minix/type.h>
#include <minix/sysutil.h>

/*===========================================================================*
 *                              get_randomness                               *
 *===========================================================================*/
void get_randomness(struct k_randomness *rand, int source)
{
	int r_next;
	unsigned long cycles;

	if (rand == NULL)
		return;

	source %= RANDOM_SOURCES;
	if (rand->bin[source].r_size >= RANDOM_ELEMENTS)
		return;

	r_next = rand->bin[source].r_next;

	/* Read cycle counter and use the low bits as entropy. */
	__asm__ __volatile__("rdcycle %0" : "=r"(cycles));
	rand->bin[source].r_buf[r_next] = (rand_t)cycles;

	if (rand->bin[source].r_size < RANDOM_ELEMENTS)
		rand->bin[source].r_size++;

	rand->bin[source].r_next = (r_next + 1) % RANDOM_ELEMENTS;
}
