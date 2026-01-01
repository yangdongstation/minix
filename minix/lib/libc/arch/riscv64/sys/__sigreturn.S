/*
 * RISC-V 64 signal return trampoline
 *
 * This code is called when returning from a signal handler
 */

#include <machine/asm.h>

/*
 * void __sigreturn(void)
 *
 * Return from signal handler
 */
ENTRY(__sigreturn)
    REG_L   a0, 0(sp)
    addi    sp, sp, SZREG
    call    _C_LABEL(sigreturn)
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
    REG_L   a0, 0(sp)
    addi    sp, sp, SZREG
    call    _C_LABEL(sigreturn)
    /* Should not return */
1:
    j       1b
END(__sigtramp)
