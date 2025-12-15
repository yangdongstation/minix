/*
 * RISC-V 64 break size tracking
 */

#include <machine/asm.h>

    .data
    .globl  __brksize
    .type   __brksize, @object
    .align  3
__brksize:
    .quad   0
