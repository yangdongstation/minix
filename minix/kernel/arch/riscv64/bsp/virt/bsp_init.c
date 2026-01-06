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
#include <stdint.h>
#include <string.h>

/* Device tree blob pointer (set by boot code) */
extern u64_t _boot_fdt;

/* Memory layout */
phys_bytes bsp_mem_start = 0x80000000UL;
phys_bytes bsp_mem_size = 128 * 1024 * 1024;  /* Default 128MB */

/* CPU info */
int bsp_num_cpus = 1;
u64_t bsp_timer_freq = 10000000;  /* 10 MHz default */

struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

#define FDT_MAGIC       0xd00dfeed
#define FDT_BEGIN_NODE  0x1
#define FDT_END_NODE    0x2
#define FDT_PROP        0x3
#define FDT_NOP         0x4
#define FDT_END         0x9

static uint32_t fdt32_to_cpu(uint32_t v)
{
    return __builtin_bswap32(v);
}

static size_t fdt_strnlen(const char *s, size_t maxlen)
{
    size_t len;

    for (len = 0; len < maxlen; len++) {
        if (s[len] == '\0')
            break;
    }

    return len;
}

static uint64_t fdt_read_cells(const uint32_t *cells, int count)
{
    uint64_t val = 0;
    int i;

    for (i = 0; i < count; i++) {
        val <<= 32;
        val |= fdt32_to_cpu(cells[i]);
    }

    return val;
}

static void parse_fdt(void *fdt)
{
    static int parsed;
    const struct fdt_header *hdr = (const struct fdt_header *)fdt;
    const uint8_t *base = (const uint8_t *)fdt;
    const uint32_t *p;
    const uint32_t *end;
    const char *strings;
    uint32_t totalsize;
    uint32_t off_struct;
    uint32_t off_strings;
    uint32_t size_struct;
    int depth = 0;
    int addr_cells = 2;
    int size_cells = 2;
    int in_memory = 0;
    int in_cpus = 0;
    int in_cpu = 0;
    int cpu_has_device_type = 0;
    int cpu_is_cpu = 1;
    int cpu_disabled = 0;
    int cpu_count = 0;
    int got_mem = 0;
    u64_t timebase = 0;

    if (parsed || fdt == NULL)
        return;
    parsed = 1;

    if (fdt32_to_cpu(hdr->magic) != FDT_MAGIC)
        return;

    totalsize = fdt32_to_cpu(hdr->totalsize);
    off_struct = fdt32_to_cpu(hdr->off_dt_struct);
    off_strings = fdt32_to_cpu(hdr->off_dt_strings);
    size_struct = fdt32_to_cpu(hdr->size_dt_struct);
    if (off_struct + size_struct > totalsize)
        return;
    if (off_strings >= totalsize)
        return;

    p = (const uint32_t *)(base + off_struct);
    end = (const uint32_t *)(base + off_struct + size_struct);
    strings = (const char *)(base + off_strings);

    while (p < end) {
        uint32_t token = fdt32_to_cpu(*p++);

        switch (token) {
        case FDT_BEGIN_NODE: {
            const char *name = (const char *)p;
            size_t maxlen = (const char *)end - name;
            size_t len = fdt_strnlen(name, maxlen);
            size_t skip = (len + 4) & ~(size_t)3;

            p = (const uint32_t *)((const char *)name + skip);
            depth++;

            if (depth == 2) {
                in_memory = (strncmp(name, "memory", 6) == 0);
                in_cpus = (strcmp(name, "cpus") == 0);
            } else if (in_cpus && depth == 3 &&
                strncmp(name, "cpu@", 4) == 0) {
                in_cpu = 1;
                cpu_has_device_type = 0;
                cpu_is_cpu = 1;
                cpu_disabled = 0;
            }
            break;
        }
        case FDT_END_NODE:
            if (in_cpu && depth == 3) {
                if (!cpu_disabled &&
                    (!cpu_has_device_type || cpu_is_cpu)) {
                    cpu_count++;
                }
                in_cpu = 0;
            }
            depth--;
            if (depth < 3)
                in_cpu = 0;
            if (depth < 2) {
                in_memory = 0;
                in_cpus = 0;
            }
            break;
        case FDT_PROP: {
            uint32_t len = fdt32_to_cpu(p[0]);
            uint32_t nameoff = fdt32_to_cpu(p[1]);
            const char *pname = strings + nameoff;
            const uint8_t *data = (const uint8_t *)&p[2];

            p += 2 + ((len + 3) / 4);

            if (depth == 1 && len >= sizeof(uint32_t)) {
                if (strcmp(pname, "#address-cells") == 0)
                    addr_cells = fdt32_to_cpu(*(const uint32_t *)data);
                else if (strcmp(pname, "#size-cells") == 0)
                    size_cells = fdt32_to_cpu(*(const uint32_t *)data);
            }

            if (in_memory && strcmp(pname, "reg") == 0 &&
                !got_mem &&
                addr_cells > 0 && addr_cells <= 2 &&
                size_cells > 0 && size_cells <= 2 &&
                len >= sizeof(uint32_t) * (addr_cells + size_cells)) {
                const uint32_t *cells = (const uint32_t *)data;
                u64_t base_addr = fdt_read_cells(cells, addr_cells);
                u64_t mem_size = fdt_read_cells(cells + addr_cells, size_cells);

                if (mem_size > 0) {
                    bsp_mem_start = (phys_bytes)base_addr;
                    bsp_mem_size = (phys_bytes)mem_size;
                    got_mem = 1;
                }
            }

            if (in_cpus && depth == 2 &&
                strcmp(pname, "timebase-frequency") == 0 &&
                len >= sizeof(uint32_t)) {
                timebase = fdt32_to_cpu(*(const uint32_t *)data);
            }

            if (in_cpu) {
                if (strcmp(pname, "device_type") == 0) {
                    cpu_has_device_type = 1;
                    cpu_is_cpu = (len >= 3 &&
                        strncmp((const char *)data, "cpu", 3) == 0);
                } else if (strcmp(pname, "status") == 0) {
                    cpu_disabled = (len >= 8 &&
                        strncmp((const char *)data, "disabled", 8) == 0);
                }
            }
            break;
        }
        case FDT_NOP:
            break;
        case FDT_END:
            p = end;
            break;
        default:
            p = end;
            break;
        }
    }

    if (cpu_count > 0)
        bsp_num_cpus = cpu_count;
    if (timebase != 0)
        bsp_timer_freq = timebase;
}

/*
 * Parse memory size from device tree
 */
static void parse_memory(void *fdt)
{
    parse_fdt(fdt);
}

/*
 * Parse CPU info from device tree
 */
static void parse_cpus(void *fdt)
{
    parse_fdt(fdt);
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
