/*
 * Test RISC-V timer operations
 *
 * Tests timer CSR and SBI timer extension
 */

#include <stdio.h>
#include <stdint.h>

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

/* Read time CSR */
static inline uint64_t read_time(void)
{
    uint64_t time;
    __asm__ __volatile__("csrr %0, time" : "=r"(time));
    return time;
}

/* Read cycle CSR */
static inline uint64_t read_cycle(void)
{
    uint64_t cycle;
    __asm__ __volatile__("csrr %0, cycle" : "=r"(cycle));
    return cycle;
}

/* Read instret CSR (instructions retired) */
static inline uint64_t read_instret(void)
{
    uint64_t instret;
    __asm__ __volatile__("csrr %0, instret" : "=r"(instret));
    return instret;
}

/* Delay loop */
static void delay(int count)
{
    for (volatile int i = 0; i < count; i++) {
        __asm__ __volatile__("nop");
    }
}

void test_time_csr(void)
{
    uint64_t t1, t2, t3;

    t1 = read_time();
    delay(10000);
    t2 = read_time();
    delay(10000);
    t3 = read_time();

    TEST("time CSR increments", t2 > t1 && t3 > t2);
    printf("  t1 = %lu\n", t1);
    printf("  t2 = %lu (delta = %lu)\n", t2, t2 - t1);
    printf("  t3 = %lu (delta = %lu)\n", t3, t3 - t2);

    /* Check that time is roughly monotonic and reasonable */
    uint64_t delta1 = t2 - t1;
    uint64_t delta2 = t3 - t2;

    /* Deltas should be similar (within 10x of each other) */
    int similar = (delta1 > 0 && delta2 > 0 &&
                   delta1 < delta2 * 10 && delta2 < delta1 * 10);
    TEST("time deltas consistent", similar);
}

void test_cycle_csr(void)
{
    uint64_t c1, c2;

    c1 = read_cycle();
    delay(1000);
    c2 = read_cycle();

    TEST("cycle CSR increments", c2 > c1);
    printf("  cycles elapsed = %lu\n", c2 - c1);
}

void test_instret_csr(void)
{
    uint64_t i1, i2;

    i1 = read_instret();

    /* Execute some instructions */
    __asm__ __volatile__(
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        ::: "memory"
    );

    i2 = read_instret();

    TEST("instret CSR increments", i2 > i1);
    printf("  instructions retired = %lu\n", i2 - i1);
}

void test_timer_frequency(void)
{
    /*
     * QEMU virt platform uses 10MHz timer frequency
     * Try to estimate it
     */
    uint64_t t1, t2;
    uint64_t c1, c2;

    t1 = read_time();
    c1 = read_cycle();

    /* Wait for some time */
    delay(100000);

    t2 = read_time();
    c2 = read_cycle();

    uint64_t time_delta = t2 - t1;
    uint64_t cycle_delta = c2 - c1;

    printf("  time delta = %lu\n", time_delta);
    printf("  cycle delta = %lu\n", cycle_delta);

    if (cycle_delta > 0) {
        uint64_t ratio = cycle_delta / time_delta;
        printf("  cycle/time ratio ~ %lu\n", ratio);
    }

    TEST("timer frequency measurable", time_delta > 0 && cycle_delta > 0);
}

void test_rdtime_pseudo(void)
{
    /*
     * Test the RDTIME pseudo-instruction (alias for csrr rd, time)
     */
    uint64_t t1, t2;

    __asm__ __volatile__("rdtime %0" : "=r"(t1));
    delay(1000);
    __asm__ __volatile__("rdtime %0" : "=r"(t2));

    TEST("rdtime pseudo-instruction", t2 > t1);
}

void test_rdcycle_pseudo(void)
{
    /*
     * Test the RDCYCLE pseudo-instruction
     */
    uint64_t c1, c2;

    __asm__ __volatile__("rdcycle %0" : "=r"(c1));
    delay(1000);
    __asm__ __volatile__("rdcycle %0" : "=r"(c2));

    TEST("rdcycle pseudo-instruction", c2 > c1);
}

void test_rdinstret_pseudo(void)
{
    /*
     * Test the RDINSTRET pseudo-instruction
     */
    uint64_t i1, i2;

    __asm__ __volatile__("rdinstret %0" : "=r"(i1));
    __asm__ __volatile__("nop\nnop\nnop" ::: "memory");
    __asm__ __volatile__("rdinstret %0" : "=r"(i2));

    TEST("rdinstret pseudo-instruction", i2 > i1);
}

int main(void)
{
    printf("=== RISC-V Timer Test Suite ===\n\n");

    printf("--- time CSR test ---\n");
    test_time_csr();

    printf("\n--- cycle CSR test ---\n");
    test_cycle_csr();

    printf("\n--- instret CSR test ---\n");
    test_instret_csr();

    printf("\n--- Timer frequency test ---\n");
    test_timer_frequency();

    printf("\n--- rdtime pseudo test ---\n");
    test_rdtime_pseudo();

    printf("\n--- rdcycle pseudo test ---\n");
    test_rdcycle_pseudo();

    printf("\n--- rdinstret pseudo test ---\n");
    test_rdinstret_pseudo();

    printf("\n=== Results ===\n");
    printf("Total: %d, Pass: %d, Fail: %d\n", test_count, pass_count, fail_count);

    return fail_count > 0 ? 1 : 0;
}
