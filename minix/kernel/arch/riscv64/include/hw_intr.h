/*
 * RISC-V 64 hardware interrupt definitions
 */

#ifndef _RISCV64_HW_INTR_H
#define _RISCV64_HW_INTR_H

#include "kernel/kernel.h"

/* Interrupt types */
#define INTR_TYPE_PLIC      0   /* External interrupt via PLIC */
#define INTR_TYPE_TIMER     1   /* Timer interrupt */
#define INTR_TYPE_SOFTWARE  2   /* Software interrupt (IPI) */

/* PLIC register offsets */
#define PLIC_PRIORITY_BASE      0x000000
#define PLIC_PENDING_BASE       0x001000
#define PLIC_ENABLE_BASE        0x002000
#define PLIC_THRESHOLD_BASE     0x200000
#define PLIC_CLAIM_BASE         0x200004

/* Per-hart context stride */
#define PLIC_CONTEXT_STRIDE     0x1000
#define PLIC_ENABLE_STRIDE      0x80

/* Each hart has two contexts: M-mode and S-mode */
#define PLIC_CONTEXT_M(hart)    ((hart) * 2)
#define PLIC_CONTEXT_S(hart)    ((hart) * 2 + 1)

/* IRQ to PLIC source mapping for QEMU virt */
#define PLIC_IRQ_UART0          10
#define PLIC_IRQ_VIRTIO(n)      (1 + (n))  /* VirtIO devices: IRQ 1-8 */

void irq_handle(int irq);

void hw_intr_mask(int irq);
void hw_intr_unmask(int irq);
void hw_intr_ack(int irq);
void hw_intr_used(int irq);
void hw_intr_not_used(int irq);
void hw_intr_disable_all(void);

#endif /* _RISCV64_HW_INTR_H */
