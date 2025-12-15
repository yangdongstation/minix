/*
 * RISC-V 64 kernel call interface
 *
 * Used for kernel calls (as opposed to IPC)
 */

#include <minix/ipcconst.h>
#include <machine/asm.h>

/*
 * int _do_kernel_call_intr(message *m_ptr)
 *
 * Perform a kernel call
 */
ENTRY(_do_kernel_call_intr)
    /* a0 = message pointer (already in place) */
    li      a7, KERVEC_INTR     /* a7 = kernel call vector */
    ecall                       /* trap to kernel */
    ret                         /* a0 = return value */
END(_do_kernel_call_intr)
