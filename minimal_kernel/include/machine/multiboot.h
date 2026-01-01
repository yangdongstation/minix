/*
 * Dummy Multiboot header for RISC-V
 * RISC-V uses device tree, not multiboot
 */

#ifndef _MACHINE_MULTIBOOT_H_
#define _MACHINE_MULTIBOOT_H_

/* Placeholder - RISC-V uses device tree */
typedef struct multiboot_info {
    uint32_t flags;
} multiboot_info_t;

#endif /* _MACHINE_MULTIBOOT_H_ */