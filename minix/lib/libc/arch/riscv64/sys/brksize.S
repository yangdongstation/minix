/*
 * RISC-V 64 break size tracking
 */

#include <machine/asm.h>

    .data
    .globl  _end
    .globl  _brksize
    .type   _brksize, @object
    .align  3
_brksize:
    .quad   _end
