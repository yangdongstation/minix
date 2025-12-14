/*
 * RISC-V 64 serial device I/O
 *
 * RISC-V uses memory-mapped I/O, not port I/O
 * These functions are for compatibility with MINIX drivers
 */

#include <lib.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>

/*
 * Read from MMIO address
 */
int sys_insb(u32_t port, endpoint_t ep, void *buf, int count)
{
    /* RISC-V doesn't have port I/O, use MMIO */
    volatile u8_t *mmio = (volatile u8_t *)(uintptr_t)port;
    u8_t *p = (u8_t *)buf;
    int i;

    for (i = 0; i < count; i++) {
        p[i] = *mmio;
    }

    return OK;
}

/*
 * Write to MMIO address
 */
int sys_outsb(u32_t port, endpoint_t ep, void *buf, int count)
{
    volatile u8_t *mmio = (volatile u8_t *)(uintptr_t)port;
    u8_t *p = (u8_t *)buf;
    int i;

    for (i = 0; i < count; i++) {
        *mmio = p[i];
    }

    return OK;
}

/*
 * Read word from MMIO
 */
int sys_insw(u32_t port, endpoint_t ep, void *buf, int count)
{
    volatile u16_t *mmio = (volatile u16_t *)(uintptr_t)port;
    u16_t *p = (u16_t *)buf;
    int i;

    for (i = 0; i < count; i++) {
        p[i] = *mmio;
    }

    return OK;
}

/*
 * Write word to MMIO
 */
int sys_outsw(u32_t port, endpoint_t ep, void *buf, int count)
{
    volatile u16_t *mmio = (volatile u16_t *)(uintptr_t)port;
    u16_t *p = (u16_t *)buf;
    int i;

    for (i = 0; i < count; i++) {
        *mmio = p[i];
    }

    return OK;
}

/*
 * Read dword from MMIO
 */
int sys_insl(u32_t port, endpoint_t ep, void *buf, int count)
{
    volatile u32_t *mmio = (volatile u32_t *)(uintptr_t)port;
    u32_t *p = (u32_t *)buf;
    int i;

    for (i = 0; i < count; i++) {
        p[i] = *mmio;
    }

    return OK;
}

/*
 * Write dword to MMIO
 */
int sys_outsl(u32_t port, endpoint_t ep, void *buf, int count)
{
    volatile u32_t *mmio = (volatile u32_t *)(uintptr_t)port;
    u32_t *p = (u32_t *)buf;
    int i;

    for (i = 0; i < count; i++) {
        *mmio = p[i];
    }

    return OK;
}
