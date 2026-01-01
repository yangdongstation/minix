/*
 * RISC-V 64 Interrupt Definitions
 */

#ifndef _MACHINE_INTERRUPT_H_
#define _MACHINE_INTERRUPT_H_

/* Interrupt types */
#define INTR_TYPE_IRQ   0
#define INTR_TYPE_SOFT  1
#define INTR_TYPE_TIMER 2

/* Interrupt priority levels */
#define IPL_NONE    0
#define IPL_SOFT    1
#define IPL_BIO     2
#define IPL_NET     3
#define IPL_TTY     4
#define IPL_VM      5
#define IPL_CLOCK   6
#define IPL_HIGH    7

#endif /* _MACHINE_INTERRUPT_H_ */