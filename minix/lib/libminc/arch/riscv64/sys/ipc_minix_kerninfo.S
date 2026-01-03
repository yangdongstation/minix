/*
 * RISC-V 64 kernel info retrieval
 */

#include <minix/ipcconst.h>
#include <machine/asm.h>

/*
 * int _minix_kerninfo(struct minix_kerninfo **kinfoptr)
 *
 * Get pointer to kernel info structure
 */
ENTRY(_minix_kerninfo)
    /* Save argument */
    mv      t0, a0              /* t0 = pointer to store result */

    /* Call kernel to get info via IPC */
    li      a0, 0               /* no endpoint */
    li      a1, 0               /* no message */
    li      a2, MINIX_KERNINFO  /* IPC request */
    li      a7, IPCVEC_INTR     /* IPC syscall vector */
    ecall

    /* Store result if pointer is not NULL */
    beqz    t0, 1f
    sd      a1, 0(t0)           /* *kinfoptr = a1 */
1:
    ret                         /* a0 = return value */
END(_minix_kerninfo)

/*
 * Alternative entry point for compatibility
 */
ENTRY(ipc_minix_kerninfo)
    j       _minix_kerninfo
END(ipc_minix_kerninfo)
