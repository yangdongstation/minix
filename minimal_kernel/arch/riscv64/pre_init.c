/*
 * Early initialization for RISC-V 64-bit
 * This runs before main() and sets up essential hardware
 */

#include "kernel/kernel.h"
#include "arch_proto.h"

/*===========================================================================*
 *				kernel_startup			     *
 *===========================================================================*/
void kernel_startup(void)
{
    /* Initialize direct console early for debugging output */
    direct_init();
}