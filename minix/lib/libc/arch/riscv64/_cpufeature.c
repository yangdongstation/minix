/*
 * RISC-V 64 CPU feature detection
 *
 * Unlike x86, RISC-V doesn't have CPUID. Features are detected
 * via device tree or ISA string parsing.
 */

#include <stdint.h>
#include <minix/cpufeature.h>

/* CPU features bitmap */
static uint32_t cpu_features = 0;

/*
 * Initialize CPU features
 * Called once during startup
 */
void _cpufeature_init(void)
{
    /* Basic RISC-V features - assume RV64GC */
    cpu_features = 0;

    /* TODO: Parse ISA string from device tree */
    /* For now, assume standard QEMU virt features */
}

/*
 * Check if a CPU feature is available
 */
int _cpufeature(int feature)
{
    switch (feature) {
    case _CPUF_I386_FPU:
        /* RISC-V F/D extensions (floating point) */
        return 1;  /* Assume present on RV64GC */

    case _CPUF_I386_HTT:
        /* SMP is handled differently on RISC-V */
        return 0;

    case _CPUF_I386_PSE:
    case _CPUF_I386_PGE:
    case _CPUF_I386_APIC_ON_CHIP:
    case _CPUF_I386_TSC:
    case _CPUF_I386_SSE1234_12:
    case _CPUF_I386_FXSR:
    case _CPUF_I386_SSE:
    case _CPUF_I386_SSE2:
    case _CPUF_I386_SSE3:
    case _CPUF_I386_SSSE3:
    case _CPUF_I386_SSE4_1:
    case _CPUF_I386_SSE4_2:
    case _CPUF_I386_HTT_MAX_NUM:
    case _CPUF_I386_MTRR:
    case _CPUF_I386_SYSENTER:
    case _CPUF_I386_SYSCALL:
    case _CPUF_I386_PAE:
        /* i386-specific features are not present on RISC-V. */
        return 0;

    default:
        return 0;
    }
}

/*
 * Get CPU vendor string
 */
const char *_cpuvendor(void)
{
    return "RISC-V";
}

/*
 * Get CPU model string
 */
const char *_cpumodel(void)
{
    return "RV64GC";
}
