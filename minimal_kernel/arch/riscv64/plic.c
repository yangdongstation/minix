/*
 * RISC-V PLIC (Platform-Level Interrupt Controller) driver
 *
 * The PLIC aggregates external interrupts from various sources and
 * delivers them to hart contexts. Each hart has M-mode and S-mode contexts.
 *
 * Key registers (per source):
 * - Priority: 0x000000 + 4*source
 * - Pending:  0x001000 + 4*(source/32)
 *
 * Key registers (per context):
 * - Enable:   0x002000 + context*0x80 + 4*(source/32)
 * - Threshold: 0x200000 + context*0x1000
 * - Claim/Complete: 0x200004 + context*0x1000
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "hw_intr.h"

/* PLIC base address (QEMU virt) */
static volatile u32_t *plic_base;

/* Maximum number of interrupt sources */
#define PLIC_MAX_SOURCES    1024
#define PLIC_MAX_CONTEXTS   15872

/* Number of interrupt sources (QEMU virt has ~96) */
#define PLIC_NUM_SOURCES    128

/* Per-IRQ CPU mask for SMP routing (key new code ~20 lines) */
static u32_t irq_cpu_mask[PLIC_NUM_SOURCES];

/*
 * Get S-mode context ID for a hart
 * QEMU virt: context = hart * 2 + 1 (S-mode)
 */
static inline int plic_context(int cpu)
{
    return cpu * 2 + 1;  /* S-mode context */
}

/*
 * Read PLIC register
 */
static inline u32_t plic_read(unsigned long offset)
{
    return *(volatile u32_t *)((unsigned long)plic_base + offset);
}

/*
 * Write PLIC register
 */
static inline void plic_write(unsigned long offset, u32_t value)
{
    *(volatile u32_t *)((unsigned long)plic_base + offset) = value;
}

/*
 * Initialize PLIC
 */
void plic_init(void)
{
    int i;

    /* Map PLIC base address */
    plic_base = (volatile u32_t *)PLIC_BASE;

    /* Set all priorities to 0 (disabled) */
    for (i = 1; i < PLIC_NUM_SOURCES; i++) {
        plic_write(PLIC_PRIORITY_BASE + i * 4, 0);
    }

    /* Initialize CPU masks - default to CPU 0 only */
    for (i = 0; i < PLIC_NUM_SOURCES; i++) {
        irq_cpu_mask[i] = 1;  /* CPU 0 */
    }

    /* Disable all interrupts for context 0 (boot CPU S-mode) */
    for (i = 0; i < (PLIC_NUM_SOURCES + 31) / 32; i++) {
        plic_write(PLIC_ENABLE_BASE + plic_context(0) * PLIC_ENABLE_STRIDE + i * 4, 0);
    }

    /* Set threshold to 0 (accept all priority levels > 0) */
    plic_set_threshold(0, 0);
}

/*
 * Enable an IRQ for a specific CPU
 */
void plic_enable_irq(int irq, int cpu)
{
    int context = plic_context(cpu);
    unsigned long enable_offset;
    u32_t enable_word;
    int word_index, bit_index;

    if (irq <= 0 || irq >= PLIC_NUM_SOURCES)
        return;

    word_index = irq / 32;
    bit_index = irq % 32;
    enable_offset = PLIC_ENABLE_BASE + context * PLIC_ENABLE_STRIDE + word_index * 4;

    enable_word = plic_read(enable_offset);
    enable_word |= (1U << bit_index);
    plic_write(enable_offset, enable_word);

    /* Set default priority if not set */
    if (plic_read(PLIC_PRIORITY_BASE + irq * 4) == 0) {
        plic_set_priority(irq, 1);
    }
}

/*
 * Disable an IRQ globally
 */
void plic_disable_irq(int irq)
{
    int cpu, context;
    unsigned long enable_offset;
    u32_t enable_word;
    int word_index, bit_index;

    if (irq <= 0 || irq >= PLIC_NUM_SOURCES)
        return;

    word_index = irq / 32;
    bit_index = irq % 32;

    /* Disable on all CPUs */
    for (cpu = 0; cpu < CONFIG_MAX_CPUS; cpu++) {
        context = plic_context(cpu);
        enable_offset = PLIC_ENABLE_BASE + context * PLIC_ENABLE_STRIDE + word_index * 4;

        enable_word = plic_read(enable_offset);
        enable_word &= ~(1U << bit_index);
        plic_write(enable_offset, enable_word);
    }

    /* Set priority to 0 */
    plic_write(PLIC_PRIORITY_BASE + irq * 4, 0);
}

/*
 * Claim an interrupt (read pending IRQ)
 * Returns IRQ number or 0 if no pending interrupt
 */
int plic_claim(int cpu)
{
    int context = plic_context(cpu);
    return plic_read(PLIC_THRESHOLD_BASE + context * PLIC_CONTEXT_STRIDE + 4);
}

/*
 * Complete an interrupt (signal EOI)
 */
void plic_complete(int cpu, int irq)
{
    int context = plic_context(cpu);
    plic_write(PLIC_THRESHOLD_BASE + context * PLIC_CONTEXT_STRIDE + 4, irq);
}

/*
 * Set interrupt priority (1-7, higher = more urgent)
 */
void plic_set_priority(int irq, int priority)
{
    if (irq <= 0 || irq >= PLIC_NUM_SOURCES)
        return;
    if (priority < 0)
        priority = 0;
    if (priority > 7)
        priority = 7;
    plic_write(PLIC_PRIORITY_BASE + irq * 4, priority);
}

/*
 * Set priority threshold for a CPU
 * Interrupts with priority <= threshold are masked
 */
void plic_set_threshold(int cpu, int threshold)
{
    int context = plic_context(cpu);
    if (threshold < 0)
        threshold = 0;
    if (threshold > 7)
        threshold = 7;
    plic_write(PLIC_THRESHOLD_BASE + context * PLIC_CONTEXT_STRIDE, threshold);
}

/*
 * Set CPU mask for an IRQ (key function for SMP support)
 *
 * This is one of the ~500 lines of new code mentioned in the plan.
 * It enables routing external interrupts to specific CPUs for
 * load balancing and CPU affinity.
 *
 * cpu_mask: bitmask of CPUs that should handle this IRQ
 *           bit 0 = CPU 0, bit 1 = CPU 1, etc.
 */
void plic_irq_cpu_mask(int irq, u32_t cpu_mask)
{
    int cpu, context;
    unsigned long enable_offset;
    u32_t enable_word;
    int word_index, bit_index;
    u32_t old_mask;

    if (irq <= 0 || irq >= PLIC_NUM_SOURCES)
        return;

    /* Save the new mask */
    old_mask = irq_cpu_mask[irq];
    irq_cpu_mask[irq] = cpu_mask;

    word_index = irq / 32;
    bit_index = irq % 32;

    /* Update enable bits for each CPU */
    for (cpu = 0; cpu < CONFIG_MAX_CPUS; cpu++) {
        context = plic_context(cpu);
        enable_offset = PLIC_ENABLE_BASE + context * PLIC_ENABLE_STRIDE + word_index * 4;

        enable_word = plic_read(enable_offset);

        if (cpu_mask & (1U << cpu)) {
            /* Enable IRQ on this CPU */
            enable_word |= (1U << bit_index);
        } else {
            /* Disable IRQ on this CPU */
            enable_word &= ~(1U << bit_index);
        }

        plic_write(enable_offset, enable_word);
    }
}

/*
 * Get current CPU mask for an IRQ
 */
u32_t plic_irq_get_cpu_mask(int irq)
{
    if (irq <= 0 || irq >= PLIC_NUM_SOURCES)
        return 0;
    return irq_cpu_mask[irq];
}

/*
 * Initialize PLIC for a secondary CPU
 * Called during SMP boot
 */
#ifdef CONFIG_SMP
void plic_init_cpu(int cpu)
{
    int i;
    int context = plic_context(cpu);

    /* Disable all interrupts for this context */
    for (i = 0; i < (PLIC_NUM_SOURCES + 31) / 32; i++) {
        plic_write(PLIC_ENABLE_BASE + context * PLIC_ENABLE_STRIDE + i * 4, 0);
    }

    /* Set threshold to 0 */
    plic_set_threshold(cpu, 0);

    /* Re-apply IRQ masks to enable interrupts for this CPU */
    for (i = 1; i < PLIC_NUM_SOURCES; i++) {
        if (irq_cpu_mask[i] & (1U << cpu)) {
            plic_enable_irq(i, cpu);
        }
    }
}
#endif /* CONFIG_SMP */
