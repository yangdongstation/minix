/*
 * RISC-V 64 clock definitions
 */

#ifndef _RISCV64_ARCH_CLOCK_H
#define _RISCV64_ARCH_CLOCK_H

/* Timer uses SBI set_timer */
#define TIMER_FREQ      CLOCK_FREQ  /* 10 MHz for QEMU virt */

void arch_timer_int_handler(void);

#endif /* _RISCV64_ARCH_CLOCK_H */
