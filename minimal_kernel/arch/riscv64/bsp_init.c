/*
 * QEMU virt platform BSP initialization
 *
 * Memory map for QEMU virt:
 *   0x00001000 - Boot ROM
 *   0x02000000 - CLINT (Core Local Interruptor)
 *   0x0C000000 - PLIC (Platform-Level Interrupt Controller)
 *   0x10000000 - UART0 (NS16550A)
 *   0x10001000 - VirtIO MMIO devices
 *   0x80000000 - RAM start
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/* Device tree blob pointer (set by boot code) */
extern u64_t _boot_fdt;

/* Memory layout */
phys_bytes bsp_mem_start = 0x80000000UL;
phys_bytes bsp_mem_size = 128 * 1024 * 1024;  /* Default 128MB */

/* CPU info */
int bsp_num_cpus = 1;
u64_t bsp_timer_freq = 10000000;  /* 10 MHz default */

/*
 * Parse memory size from device tree (simplified)
 */
static void parse_memory(void *fdt)
{
    /* TODO: Proper FDT parsing */
    /* For now, use defaults for QEMU -m 128M */
    (void)fdt;

    bsp_mem_start = 0x80000000UL;
    bsp_mem_size = 128 * 1024 * 1024;
}

/*
 * Parse CPU info from device tree
 */
static void parse_cpus(void *fdt)
{
    /* TODO: Proper FDT parsing */
    (void)fdt;

    bsp_num_cpus = 1;  /* Default single core */
    bsp_timer_freq = 10000000;  /* 10 MHz for QEMU */
}

/*
 * Early BSP initialization (before memory management)
 */
void bsp_early_init(void)
{
    void *fdt = (void *)_boot_fdt;

    /* Initialize SBI console for early output */
    direct_print("MINIX/riscv64 BSP early init\n");

    /* Parse device tree */
    if (fdt != NULL) {
        parse_memory(fdt);
        parse_cpus(fdt);
    }

    direct_print("Memory: ");
    direct_print_hex(bsp_mem_start);
    direct_print(" - ");
    direct_print_hex(bsp_mem_start + bsp_mem_size);
    direct_print("\n");
}

/*
 * BSP initialization (after memory management)
 */
void bsp_init(void)
{
    direct_print("BSP init: QEMU virt platform\n");

    /* Set timer frequency */
    arch_set_timer_freq(bsp_timer_freq);

    /* Initialize PLIC */
    plic_init();

    /* Initialize serial (NS16550A UART) */
    bsp_serial_init();
}

/*
 * Get physical memory range
 */
void bsp_get_memory(phys_bytes *start, phys_bytes *size)
{
    *start = bsp_mem_start;
    *size = bsp_mem_size;
}

/*
 * Get number of CPUs
 */
int bsp_get_num_cpus(void)
{
    return bsp_num_cpus;
}

/*
 * Get timer frequency
 */
u64_t bsp_get_timer_freq(void)
{
    return bsp_timer_freq;
}
