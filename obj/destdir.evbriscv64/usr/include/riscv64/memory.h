/*
 * RISC-V 64 memory layout definitions
 */

#ifndef _RISCV64_MEMORY_H
#define _RISCV64_MEMORY_H

/* QEMU virt platform memory map */
#define VIRT_CLINT_BASE     0x02000000UL    /* Core Local Interruptor */
#define VIRT_CLINT_SIZE     0x00010000UL
#define VIRT_PLIC_BASE      0x0C000000UL    /* Platform Level Interrupt Controller */
#define VIRT_PLIC_SIZE      0x04000000UL
#define VIRT_UART0_BASE     0x10000000UL    /* NS16550A UART */
#define VIRT_UART0_SIZE     0x00000100UL
#define VIRT_VIRTIO_BASE    0x10001000UL    /* VirtIO MMIO devices */
#define VIRT_VIRTIO_SIZE    0x00001000UL
#define VIRT_FLASH_BASE     0x20000000UL    /* Flash memory */
#define VIRT_FLASH_SIZE     0x02000000UL
#define VIRT_RAM_BASE       0x80000000UL    /* Main memory */

/* Kernel virtual address base (high half) */
#define KERNEL_VIRT_BASE    0xFFFFFFFF80000000UL

/* User space boundaries */
#define USER_VIRT_LOW       0x0000000000400000UL    /* User space start */
#define USER_VIRT_HIGH      0x0000003FFFFFFFFFULL    /* User space end (256GB) */

/* Stack locations */
#define USER_STACK_TOP      0x0000003FF0000000UL    /* Top of user stack area */
#define KERNEL_STACK_SIZE   8192                     /* Kernel stack per process */

/* Page table constants */
#define PT_SIZE             4096    /* Page table size in bytes */
#define PT_ENTRIES          512     /* Entries per page table */
#define PT_LEVELS           3       /* Number of page table levels (Sv39) */

#endif /* _RISCV64_MEMORY_H */
