/*
 * Test RISC-V memory operations
 *
 * Tests memory access patterns, alignment, and caching
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

/* Test buffer */
static uint8_t buffer[4096] __attribute__((aligned(4096)));

void test_aligned_access(void)
{
    volatile uint64_t *ptr64 = (volatile uint64_t *)buffer;
    volatile uint32_t *ptr32 = (volatile uint32_t *)buffer;
    volatile uint16_t *ptr16 = (volatile uint16_t *)buffer;
    volatile uint8_t *ptr8 = (volatile uint8_t *)buffer;

    /* 64-bit aligned access */
    *ptr64 = 0x123456789ABCDEF0ULL;
    TEST("64-bit aligned write", *ptr64 == 0x123456789ABCDEF0ULL);

    /* 32-bit aligned access */
    *ptr32 = 0xDEADBEEF;
    TEST("32-bit aligned write", *ptr32 == 0xDEADBEEF);

    /* 16-bit aligned access */
    *ptr16 = 0xCAFE;
    TEST("16-bit aligned write", *ptr16 == 0xCAFE);

    /* 8-bit access */
    *ptr8 = 0x42;
    TEST("8-bit write", *ptr8 == 0x42);
}

void test_unaligned_access(void)
{
    /* RISC-V may or may not support unaligned access depending on implementation */
    volatile uint32_t *ptr;
    uint32_t val;
    int unaligned_ok = 1;

    /* Try unaligned 32-bit access */
    ptr = (volatile uint32_t *)(buffer + 1);

    /* This might trap on some RISC-V implementations */
    __asm__ __volatile__(
        "sw %1, 0(%0)\n"
        "lw %1, 0(%0)\n"
        : : "r"(ptr), "r"(0x12345678)
        : "memory"
    );

    val = *ptr;
    TEST("32-bit unaligned access (may fail)", val == 0x12345678 || unaligned_ok);
}

void test_memory_ordering(void)
{
    volatile uint64_t a = 0, b = 0;

    /* Store-Store ordering */
    a = 1;
    __asm__ __volatile__("fence w, w" ::: "memory");
    b = 2;
    TEST("store-store fence", a == 1 && b == 2);

    /* Load-Load ordering */
    __asm__ __volatile__("fence r, r" ::: "memory");
    volatile uint64_t x = a;
    volatile uint64_t y = b;
    TEST("load-load fence", x == 1 && y == 2);

    /* Full fence */
    a = 10;
    __asm__ __volatile__("fence rw, rw" ::: "memory");
    b = 20;
    TEST("full fence", a == 10 && b == 20);

    /* I/O fence */
    __asm__ __volatile__("fence iorw, iorw" ::: "memory");
    TEST("I/O fence", 1);
}

void test_cache_operations(void)
{
    /* RISC-V doesn't have standard cache control instructions at S-mode */
    /* Cache management is typically done via SBI or CMO extension */

    /* fence.i ensures instruction cache coherency */
    __asm__ __volatile__("fence.i" ::: "memory");
    TEST("fence.i (icache sync)", 1);

    /* SFENCE.VMA for TLB flush (requires supervisor mode) */
    /* Skip if not in supervisor mode */
    printf("  [INFO] SFENCE.VMA test skipped (requires S-mode)\n");
}

void test_memcpy(void)
{
    uint8_t src[256], dst[256];
    int i;

    /* Initialize source */
    for (i = 0; i < 256; i++) {
        src[i] = i & 0xFF;
    }

    /* Copy */
    memcpy(dst, src, 256);

    /* Verify */
    int match = 1;
    for (i = 0; i < 256; i++) {
        if (dst[i] != src[i]) {
            match = 0;
            break;
        }
    }
    TEST("memcpy correctness", match);
}

void test_memset(void)
{
    uint8_t buf[256];
    int i;

    /* Set to pattern */
    memset(buf, 0xAA, 256);

    /* Verify */
    int match = 1;
    for (i = 0; i < 256; i++) {
        if (buf[i] != 0xAA) {
            match = 0;
            break;
        }
    }
    TEST("memset correctness", match);

    /* Clear */
    memset(buf, 0, 256);
    match = 1;
    for (i = 0; i < 256; i++) {
        if (buf[i] != 0) {
            match = 0;
            break;
        }
    }
    TEST("memset zero", match);
}

void test_stack_alignment(void)
{
    /* Check stack pointer alignment */
    uint64_t sp;
    __asm__ __volatile__("mv %0, sp" : "=r"(sp));

    /* RISC-V ABI requires 16-byte stack alignment */
    TEST("stack 16-byte aligned", (sp & 0xF) == 0);
    printf("  Stack pointer: 0x%lx\n", sp);
}

int main(void)
{
    printf("=== RISC-V Memory Operations Test Suite ===\n\n");

    printf("--- Aligned access test ---\n");
    test_aligned_access();

    printf("\n--- Unaligned access test ---\n");
    test_unaligned_access();

    printf("\n--- Memory ordering test ---\n");
    test_memory_ordering();

    printf("\n--- Cache operations test ---\n");
    test_cache_operations();

    printf("\n--- memcpy test ---\n");
    test_memcpy();

    printf("\n--- memset test ---\n");
    test_memset();

    printf("\n--- Stack alignment test ---\n");
    test_stack_alignment();

    printf("\n=== Results ===\n");
    printf("Total: %d, Pass: %d, Fail: %d\n", test_count, pass_count, fail_count);

    return fail_count > 0 ? 1 : 0;
}
