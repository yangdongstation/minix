/*
 * QEMU virt platform interrupt support
 *
 * IRQ assignments for QEMU virt:
 *   1-8   : VirtIO devices
 *   10    : UART0
 *   11    : RTC (optional)
 *   32+   : PCI interrupts
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"
#include "hw_intr.h"

/* IRQ assignments */
#define IRQ_VIRTIO_START    1
#define IRQ_VIRTIO_END      8
#define IRQ_UART0           10
#define IRQ_RTC             11

/* IRQ handler table */
static void (*bsp_irq_handlers[NR_IRQ_VECTORS])(int irq);

/*
 * Initialize BSP interrupt controller
 */
void bsp_intr_init(void)
{
    int i;

    /* Clear handler table */
    for (i = 0; i < NR_IRQ_VECTORS; i++) {
        bsp_irq_handlers[i] = NULL;
    }

    /* PLIC is already initialized by plic_init() */

    /* Set priority threshold to 0 (accept all) */
    plic_set_threshold(0, 0);

    /* Set default priorities */
    plic_set_priority(IRQ_UART0, 1);
    for (i = IRQ_VIRTIO_START; i <= IRQ_VIRTIO_END; i++) {
        plic_set_priority(i, 1);
    }
}

/*
 * Register an IRQ handler
 */
int bsp_intr_register(int irq, void (*handler)(int))
{
    if (irq < 0 || irq >= NR_IRQ_VECTORS)
        return -1;

    bsp_irq_handlers[irq] = handler;
    return 0;
}

/*
 * Enable an IRQ
 */
void bsp_intr_enable(int irq)
{
    plic_enable_irq(irq, 0);  /* Enable on CPU 0 */
}

/*
 * Disable an IRQ
 */
void bsp_intr_disable(int irq)
{
    plic_disable_irq(irq);
}

/*
 * Handle external interrupt
 */
void bsp_intr_handler(void)
{
    int irq;

    /* Claim interrupt from PLIC */
    irq = plic_claim(0);

    if (irq == 0) {
        /* Spurious interrupt */
        return;
    }

    /* Call registered handler */
    if (irq < NR_IRQ_VECTORS && bsp_irq_handlers[irq] != NULL) {
        bsp_irq_handlers[irq](irq);
    }

    /* Complete interrupt */
    plic_complete(0, irq);
}

/*
 * Get IRQ for VirtIO device
 */
int bsp_virtio_irq(int device_num)
{
    if (device_num < 0 || device_num > 7)
        return -1;
    return IRQ_VIRTIO_START + device_num;
}

/*
 * Get UART IRQ
 */
int bsp_uart_irq(int uart_num)
{
    if (uart_num == 0)
        return IRQ_UART0;
    return -1;
}

#ifdef CONFIG_SMP
/*
 * Initialize interrupts for secondary CPU
 */
void bsp_intr_init_cpu(int cpu)
{
    /* Set priority threshold */
    plic_set_threshold(cpu, 0);

    /* PLIC per-CPU init */
    plic_init_cpu(cpu);
}
#endif
