/*
 * RISC-V 64-bit kernel entry point
 * Called from head.S after basic initialization
 */

#include "kernel/kernel.h"
#include "arch_proto.h"

/* External functions from other kernel modules */
extern void kmain(kinfo_t *cbi);

/*===========================================================================*
 *				kernel_main			     *
 *===========================================================================*/
void kernel_main(void)
{
    /* Initialize direct console early */
    direct_init();

    /* Call generic kernel main */
    kmain(&kinfo);
}
