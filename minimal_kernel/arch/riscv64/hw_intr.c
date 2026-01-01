/*
 * RISC-V 64 hardware interrupt handling
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"
#include "hw_intr.h"

/* Interrupt handler table */
typedef void (*irq_handler_t)(int irq);
static irq_handler_t irq_handlers[NR_IRQ_VECTORS];

/* IRQ enable/disable state */
static int irq_enabled[NR_IRQ_VECTORS];

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
    int i;

    /* Initialize PLIC */
    plic_init();

    /* Clear handler table */
    for (i = 0; i < NR_IRQ_VECTORS; i++) {
        irq_handlers[i] = NULL;
        irq_enabled[i] = 0;
    }

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

    irq_enabled[irq] = 0;
    plic_disable_irq(irq);
}

/*
 * Unmask (enable) an IRQ
 */
void hw_intr_unmask(int irq)
{
    if (irq < 0 || irq >= NR_IRQ_VECTORS)
        return;

    irq_enabled[irq] = 1;
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

    /* Call registered handler */
    if (irq > 0 && irq < NR_IRQ_VECTORS && irq_handlers[irq] != NULL) {
        irq_handlers[irq](irq);
    }

    /* Complete interrupt */
    plic_complete(CURRENT_CPU, irq);
}

/*
 * Register an IRQ handler
 */
int hw_intr_register(int irq, irq_handler_t handler)
{
    if (irq < 0 || irq >= NR_IRQ_VECTORS)
        return -1;

    irq_handlers[irq] = handler;
    return 0;
}

/*
 * Unregister an IRQ handler
 */
void hw_intr_unregister(int irq)
{
    if (irq < 0 || irq >= NR_IRQ_VECTORS)
        return;

    hw_intr_mask(irq);
    irq_handlers[irq] = NULL;
}

/*
 * Set IRQ priority
 */
void hw_intr_set_priority(int irq, int priority)
{
    plic_set_priority(irq, priority);
}

#ifdef CONFIG_SMP
/*
 * Set CPU affinity for an IRQ
 */
void hw_intr_set_affinity(int irq, u32_t cpu_mask)
{
    plic_irq_cpu_mask(irq, cpu_mask);
}
#endif
