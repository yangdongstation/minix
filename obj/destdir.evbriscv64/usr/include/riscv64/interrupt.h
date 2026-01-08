/*
 * RISC-V 64 interrupt definitions
 */

#ifndef _RISCV64_INTERRUPT_H
#define _RISCV64_INTERRUPT_H

/* Number of IRQ vectors */
#define NR_IRQ_VECTORS      128

/* Interrupt types */
#define INTR_TYPE_EXT       0       /* External (PLIC) */
#define INTR_TYPE_TIMER     1       /* Timer */
#define INTR_TYPE_SW        2       /* Software (IPI) */

/* QEMU virt platform IRQs */
#define IRQ_UART0           10
#define IRQ_VIRTIO_START    1
#define IRQ_VIRTIO_END      8
#define IRQ_RTC             11

/* IRQ for VirtIO devices */
#define IRQ_VIRTIO(n)       (IRQ_VIRTIO_START + (n))

#endif /* _RISCV64_INTERRUPT_H */
