/*
 * RISC-V 64 signal return trampoline
 *
 * This code is called when returning from a signal handler
 */

#include <machine/asm.h>
#include <sys/syscall.h>

/*
 * void __sigreturn(void)
 *
 * Return from signal handler
 */
ENTRY(__sigreturn)
    /* Load sigreturn syscall number */
    li      a7, SYS___sigreturn
    ecall

    /* Should not return, but just in case */
1:
    j       1b
END(__sigreturn)

/*
 * Signal trampoline - called by kernel after setting up signal frame
 */
ENTRY(__sigtramp)
    /* On entry:
     * sp points to signal frame
     * a0 = signal number (passed to handler)
     * ra = address of handler
     */

    /* Call signal handler */
    jalr    ra

    /* Handler returned, do sigreturn */
    /* sp still points to signal frame */
    li      a7, SYS___sigreturn
    ecall

    /* Should not return */
1:
    j       1b
END(__sigtramp)
