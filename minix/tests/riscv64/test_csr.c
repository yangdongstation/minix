/*
 * Test RISC-V CSR operations
 *
 * Tests reading and writing Control and Status Registers
 */

#include <stdio.h>
#include <stdint.h>

/* CSR read macros */
#define csr_read(csr) ({ \
    uint64_t __v; \
    __asm__ __volatile__("csrr %0, " #csr : "=r"(__v)); \
    __v; \
})

#define csr_write(csr, val) ({ \
    __asm__ __volatile__("csrw " #csr ", %0" :: "r"(val)); \
})

#define csr_set(csr, val) ({ \
    __asm__ __volatile__("csrs " #csr ", %0" :: "r"(val)); \
})

#define csr_clear(csr, val) ({ \
    __asm__ __volatile__("csrc " #csr ", %0" :: "r"(val)); \
})

static int test_count = 0;
static int pass_count = 0;
static int fail_count = 0;

#define TEST(name, cond) do { \
    test_count++; \
    if (cond) { \
        printf("[PASS] %s\n", name); \
        pass_count++; \
    } else { \
        printf("[FAIL] %s\n", name); \
        fail_count++; \
    } \
} while (0)

void test_sstatus(void)
{
    uint64_t sstatus;

    /* Read sstatus */
    sstatus = csr_read(sstatus);
    TEST("sstatus readable", 1);

    /* Check SPP bit (should be 0 in user mode, 1 in supervisor) */
    printf("  sstatus = 0x%lx\n", sstatus);
    printf("  SPP (bit 8) = %d\n", (int)((sstatus >> 8) & 1));
    printf("  SIE (bit 1) = %d\n", (int)((sstatus >> 1) & 1));
}

void test_sie(void)
{
    uint64_t sie;

    /* Read sie (Supervisor Interrupt Enable) */
    sie = csr_read(sie);
    TEST("sie readable", 1);

    printf("  sie = 0x%lx\n", sie);
    printf("  SSIE (bit 1) = %d\n", (int)((sie >> 1) & 1));  /* Software */
    printf("  STIE (bit 5) = %d\n", (int)((sie >> 5) & 1));  /* Timer */
    printf("  SEIE (bit 9) = %d\n", (int)((sie >> 9) & 1));  /* External */
}

void test_sip(void)
{
    uint64_t sip;

    /* Read sip (Supervisor Interrupt Pending) */
    sip = csr_read(sip);
    TEST("sip readable", 1);

    printf("  sip = 0x%lx\n", sip);
}

void test_satp(void)
{
    uint64_t satp;

    /* Read satp (Supervisor Address Translation and Protection) */
    satp = csr_read(satp);
    TEST("satp readable", 1);

    printf("  satp = 0x%lx\n", satp);
    printf("  MODE = %d (8=Sv39, 9=Sv48)\n", (int)(satp >> 60));
    printf("  ASID = 0x%lx\n", (satp >> 44) & 0xFFFF);
    printf("  PPN = 0x%lx\n", satp & 0xFFFFFFFFFFF);
}

void test_time(void)
{
    uint64_t time1, time2;

    /* Read time CSR */
    time1 = csr_read(time);

    /* Small delay */
    for (volatile int i = 0; i < 10000; i++);

    time2 = csr_read(time);

    TEST("time CSR incrementing", time2 > time1);
    printf("  time1 = %lu\n", time1);
    printf("  time2 = %lu\n", time2);
    printf("  delta = %lu\n", time2 - time1);
}

void test_cycle(void)
{
    uint64_t cycle1, cycle2;

    /* Read cycle CSR */
    cycle1 = csr_read(cycle);

    /* Small delay */
    for (volatile int i = 0; i < 10000; i++);

    cycle2 = csr_read(cycle);

    TEST("cycle CSR incrementing", cycle2 > cycle1);
    printf("  cycle1 = %lu\n", cycle1);
    printf("  cycle2 = %lu\n", cycle2);
    printf("  delta = %lu\n", cycle2 - cycle1);
}

void test_hartid(void)
{
    /* Note: mhartid is M-mode only, but some implementations allow reading */
    printf("  [INFO] Hart ID test skipped (M-mode only)\n");
}

int main(void)
{
    printf("=== RISC-V CSR Test Suite ===\n\n");

    printf("--- sstatus test ---\n");
    test_sstatus();

    printf("\n--- sie test ---\n");
    test_sie();

    printf("\n--- sip test ---\n");
    test_sip();

    printf("\n--- satp test ---\n");
    test_satp();

    printf("\n--- time test ---\n");
    test_time();

    printf("\n--- cycle test ---\n");
    test_cycle();

    printf("\n--- hartid test ---\n");
    test_hartid();

    printf("\n=== Results ===\n");
    printf("Total: %d, Pass: %d, Fail: %d\n", test_count, pass_count, fail_count);

    return fail_count > 0 ? 1 : 0;
}
