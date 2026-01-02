/*
 * RISC-V 64 kernel info retrieval
 */

#include <minix/ipcconst.h>
#include <machine/asm.h>

/*
 * int ipc_minix_kerninfo(struct minix_kerninfo **kinfoptr)
 *
 * Get pointer to kernel info structure.
 */
ENTRY(ipc_minix_kerninfo)
    /* Save argument. */
    mv      t0, a0              /* t0 = pointer to store result */

    /* Call kernel to get info */
    li      a0, 0               /* Request kerninfo */
    li      a7, KERVEC_INTR     /* Kernel call vector */
    ecall

    /* Store result if pointer is not NULL */
    beqz    t0, 1f
    sd      a1, 0(t0)           /* *kinfoptr = a1 */
1:
    ret                         /* a0 = return value */
END(ipc_minix_kerninfo)
