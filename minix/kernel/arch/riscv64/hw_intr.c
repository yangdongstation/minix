/*
 * RISC-V 64 hardware interrupt handling
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"
#include "hw_intr.h"

/* Current CPU (for SMP) */
#ifdef CONFIG_SMP
#define CURRENT_CPU cpu_number()
#else
#define CURRENT_CPU 0
#endif

/*
 * Initialize hardware interrupts
 */
void hw_intr_init(void)
{
    /* Initialize PLIC */
    plic_init();

    /* Enable external interrupts in SIE */
    csr_set_sie(SIE_SEIE);
}

/*
 * Mask (disable) an IRQ
 */
void hw_intr_mask(int irq)
{
    if (irq < 0 || irq >= NR_IRQ_VECTORS)
        return;

    plic_disable_irq(irq);
}

/*
 * Unmask (enable) an IRQ
 */
void hw_intr_unmask(int irq)
{
    if (irq < 0 || irq >= NR_IRQ_VECTORS)
        return;

    plic_enable_irq(irq, CURRENT_CPU);
}

/*
 * Acknowledge an IRQ (signal EOI)
 */
void hw_intr_ack(int irq)
{
    plic_complete(CURRENT_CPU, irq);
}

/*
 * Main hardware interrupt handler
 * Called from exception.c when external interrupt occurs
 */
void hw_intr_handler(struct trapframe *tf)
{
    int irq;

    /* Claim interrupt from PLIC */
    irq = plic_claim(CURRENT_CPU);

    if (irq == 0) {
        /* Spurious interrupt */
        return;
    }

    irq_handle(irq);
}

void hw_intr_used(int irq)
{
    (void)irq;
}

void hw_intr_not_used(int irq)
{
    (void)irq;
}

void hw_intr_disable_all(void)
{
    csr_clear_sie(SIE_SEIE);
}
