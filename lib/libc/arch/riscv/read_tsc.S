/*
 * RISC-V 64 cycle counter reading
 *
 * RISC-V provides rdcycle and rdtime pseudo-instructions
 */

#include <machine/asm.h>

/*
 * void read_tsc(uint32_t *lo, uint32_t *hi)
 *
 * Read cycle counter into lo and hi parts
 */
ENTRY(read_tsc)
    rdcycle t0              /* Read full 64-bit cycle counter */
    sw      t0, 0(a0)       /* Store low 32 bits */
    srli    t0, t0, 32
    sw      t0, 0(a1)       /* Store high 32 bits */
    ret
END(read_tsc)

/*
 * uint64_t read_tsc_64(void)
 *
 * Read 64-bit cycle counter
 */
ENTRY(read_tsc_64)
    rdcycle a0
    ret
END(read_tsc_64)

/*
 * uint64_t read_time_64(void)
 *
 * Read 64-bit time counter
 */
ENTRY(read_time_64)
    rdtime  a0
    ret
END(read_time_64)
