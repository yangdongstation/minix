/*
 * RISC-V 64 random number generation
 *
 * Uses various sources of entropy available on RISC-V
 */

#include <lib.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <machine/archtypes.h>

/*
 * Get randomness from system
 *
 * On RISC-V, we use the time counter and other sources
 */
int sys_getrandomness(struct k_randomness *rand)
{
    message m;
    int r;

    memset(&m, 0, sizeof(m));
    m.m_type = SYS_GETRANDOMNESS;

    r = _kernel_call(SYS_GETRANDOMNESS, &m);

    if (r == OK && rand != NULL) {
        /* Copy randomness data */
        memcpy(rand, &m.m_krandomness, sizeof(*rand));
    }

    return r;
}

/*
 * Add entropy to the system pool
 *
 * Uses cycle counter as additional entropy source
 */
void get_randomness(struct k_randomness *rand, int source)
{
    static unsigned long prev_cycles = 0;
    unsigned long cycles;

    /* Read cycle counter */
    __asm__ __volatile__("rdcycle %0" : "=r"(cycles));

    /* Mix in some entropy from cycle difference */
    if (rand != NULL && rand->random_elements < RANDOM_ELEMENTS) {
        rand->random_buf[rand->random_elements % RANDOM_ELEMENTS] ^=
            (cycles - prev_cycles);
    }

    prev_cycles = cycles;
}
