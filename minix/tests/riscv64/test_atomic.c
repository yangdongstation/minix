/*
 * Test RISC-V atomic operations
 *
 * Tests atomic memory operations (AMO) instructions
 */

#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>

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

/* Atomic add */
static inline int64_t atomic_add_64(volatile int64_t *ptr, int64_t val)
{
    int64_t ret;
    __asm__ __volatile__(
        "amoadd.d %0, %2, (%1)"
        : "=r"(ret)
        : "r"(ptr), "r"(val)
        : "memory"
    );
    return ret;
}

/* Atomic swap */
static inline int64_t atomic_swap_64(volatile int64_t *ptr, int64_t val)
{
    int64_t ret;
    __asm__ __volatile__(
        "amoswap.d %0, %2, (%1)"
        : "=r"(ret)
        : "r"(ptr), "r"(val)
        : "memory"
    );
    return ret;
}

/* Atomic and */
static inline int64_t atomic_and_64(volatile int64_t *ptr, int64_t val)
{
    int64_t ret;
    __asm__ __volatile__(
        "amoand.d %0, %2, (%1)"
        : "=r"(ret)
        : "r"(ptr), "r"(val)
        : "memory"
    );
    return ret;
}

/* Atomic or */
static inline int64_t atomic_or_64(volatile int64_t *ptr, int64_t val)
{
    int64_t ret;
    __asm__ __volatile__(
        "amoor.d %0, %2, (%1)"
        : "=r"(ret)
        : "r"(ptr), "r"(val)
        : "memory"
    );
    return ret;
}

/* Atomic xor */
static inline int64_t atomic_xor_64(volatile int64_t *ptr, int64_t val)
{
    int64_t ret;
    __asm__ __volatile__(
        "amoxor.d %0, %2, (%1)"
        : "=r"(ret)
        : "r"(ptr), "r"(val)
        : "memory"
    );
    return ret;
}

/* Load-Reserved / Store-Conditional */
static inline int lr_sc_test(volatile int64_t *ptr, int64_t expected, int64_t newval)
{
    int64_t tmp;
    int result;
    __asm__ __volatile__(
        "1: lr.d %0, (%2)\n"
        "   bne %0, %3, 2f\n"
        "   sc.d %1, %4, (%2)\n"
        "   bnez %1, 1b\n"
        "   li %1, 1\n"
        "   j 3f\n"
        "2: li %1, 0\n"
        "3:"
        : "=&r"(tmp), "=&r"(result)
        : "r"(ptr), "r"(expected), "r"(newval)
        : "memory"
    );
    return result;
}

void test_amoadd(void)
{
    volatile int64_t val = 100;
    int64_t old;

    old = atomic_add_64(&val, 50);
    TEST("amoadd.d returns old value", old == 100);
    TEST("amoadd.d updates value", val == 150);

    old = atomic_add_64(&val, -30);
    TEST("amoadd.d negative", val == 120);
}

void test_amoswap(void)
{
    volatile int64_t val = 0xDEADBEEF;
    int64_t old;

    old = atomic_swap_64(&val, 0xCAFEBABE);
    TEST("amoswap.d returns old value", old == 0xDEADBEEF);
    TEST("amoswap.d updates value", val == 0xCAFEBABE);
}

void test_amoand(void)
{
    volatile int64_t val = 0xFF00FF00;
    int64_t old;

    old = atomic_and_64(&val, 0x0F0F0F0F);
    TEST("amoand.d returns old value", old == 0xFF00FF00);
    TEST("amoand.d updates value", val == 0x0F000F00);
}

void test_amoor(void)
{
    volatile int64_t val = 0x0F000F00;
    int64_t old;

    old = atomic_or_64(&val, 0x00F000F0);
    TEST("amoor.d returns old value", old == 0x0F000F00);
    TEST("amoor.d updates value", val == 0x0FF00FF0);
}

void test_amoxor(void)
{
    volatile int64_t val = 0xAAAAAAAA;
    int64_t old;

    old = atomic_xor_64(&val, 0x55555555);
    TEST("amoxor.d returns old value", old == 0xAAAAAAAA);
    TEST("amoxor.d updates value", val == 0xFFFFFFFF);
}

void test_lr_sc(void)
{
    volatile int64_t val = 42;
    int result;

    /* Successful CAS */
    result = lr_sc_test(&val, 42, 100);
    TEST("lr/sc success case", result == 1 && val == 100);

    /* Failed CAS (wrong expected) */
    result = lr_sc_test(&val, 42, 200);
    TEST("lr/sc failure case", result == 0 && val == 100);
}

void test_fence(void)
{
    volatile int64_t a = 0, b = 0;

    a = 1;
    __asm__ __volatile__("fence rw, rw" ::: "memory");
    b = 2;

    TEST("fence instruction", a == 1 && b == 2);

    /* fence.i for instruction cache */
    __asm__ __volatile__("fence.i" ::: "memory");
    TEST("fence.i instruction", 1);
}

int main(void)
{
    printf("=== RISC-V Atomic Operations Test Suite ===\n\n");

    printf("--- amoadd test ---\n");
    test_amoadd();

    printf("\n--- amoswap test ---\n");
    test_amoswap();

    printf("\n--- amoand test ---\n");
    test_amoand();

    printf("\n--- amoor test ---\n");
    test_amoor();

    printf("\n--- amoxor test ---\n");
    test_amoxor();

    printf("\n--- lr/sc test ---\n");
    test_lr_sc();

    printf("\n--- fence test ---\n");
    test_fence();

    printf("\n=== Results ===\n");
    printf("Total: %d, Pass: %d, Fail: %d\n", test_count, pass_count, fail_count);

    return fail_count > 0 ? 1 : 0;
}
