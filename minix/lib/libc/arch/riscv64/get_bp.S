/*
 * RISC-V 64 get base pointer (frame pointer)
 */

#include <machine/asm.h>

/*
 * void *get_bp(void)
 *
 * Return current frame pointer (s0/fp)
 */
ENTRY(get_bp)
    mv      a0, s0
    ret
END(get_bp)
