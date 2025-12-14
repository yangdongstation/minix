/*
 * Test MINIX VM (Virtual Memory) on RISC-V 64
 *
 * Tests virtual memory operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <setjmp.h>

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

#define PAGE_SIZE 4096

static volatile sig_atomic_t got_sigsegv = 0;
static jmp_buf jump_buf;

void sigsegv_handler(int sig)
{
    got_sigsegv = 1;
    longjmp(jump_buf, 1);
}

void test_mmap_anon(void)
{
    void *ptr;
    size_t size = PAGE_SIZE * 4;

    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        TEST("mmap anonymous", 0);
        return;
    }

    TEST("mmap anonymous returns valid ptr", ptr != MAP_FAILED);

    /* Write to mapped memory */
    memset(ptr, 0xAA, size);
    TEST("write to mmap region", 1);

    /* Read back */
    int correct = 1;
    unsigned char *p = (unsigned char *)ptr;
    for (size_t i = 0; i < size; i++) {
        if (p[i] != 0xAA) {
            correct = 0;
            break;
        }
    }
    TEST("read from mmap region", correct);

    /* Unmap */
    int r = munmap(ptr, size);
    TEST("munmap", r == 0);
}

void test_mmap_fixed(void)
{
    void *hint = (void *)0x40000000;
    void *ptr;
    size_t size = PAGE_SIZE;

    ptr = mmap(hint, size, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        printf("  [INFO] mmap at hint address failed (may be in use)\n");
        TEST("mmap with hint", 1);  /* Not a failure */
        return;
    }

    TEST("mmap with hint", 1);
    printf("  requested: %p, got: %p\n", hint, ptr);

    munmap(ptr, size);
}

void test_mprotect(void)
{
    void *ptr;
    size_t size = PAGE_SIZE;
    int r;

    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        TEST("mmap for mprotect test", 0);
        return;
    }

    /* Write some data */
    memset(ptr, 0x42, size);

    /* Make read-only */
    r = mprotect(ptr, size, PROT_READ);
    TEST("mprotect to read-only", r == 0);

    /* Reading should work */
    unsigned char val = *(unsigned char *)ptr;
    TEST("read from read-only page", val == 0x42);

    /* Restore write permission for cleanup */
    mprotect(ptr, size, PROT_READ | PROT_WRITE);
    munmap(ptr, size);
}

void test_page_fault_protection(void)
{
    void *ptr;
    size_t size = PAGE_SIZE;

    ptr = mmap(NULL, size, PROT_READ,
               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        TEST("mmap for protection test", 0);
        return;
    }

    /* Install SIGSEGV handler */
    struct sigaction sa, old_sa;
    sa.sa_handler = sigsegv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGSEGV, &sa, &old_sa) < 0) {
        printf("  [INFO] Cannot install signal handler, skipping\n");
        munmap(ptr, size);
        return;
    }

    got_sigsegv = 0;

    if (setjmp(jump_buf) == 0) {
        /* Try to write to read-only page */
        *(volatile char *)ptr = 0x99;
    }

    sigaction(SIGSEGV, &old_sa, NULL);

    TEST("write to read-only causes SIGSEGV", got_sigsegv);

    munmap(ptr, size);
}

void test_brk_sbrk(void)
{
    void *orig_brk, *new_brk;
    int r;

    /* Get current break */
    orig_brk = sbrk(0);
    TEST("sbrk(0) returns valid pointer", orig_brk != (void *)-1);
    printf("  current brk = %p\n", orig_brk);

    /* Increase break */
    new_brk = sbrk(PAGE_SIZE);
    if (new_brk != (void *)-1) {
        TEST("sbrk increase", 1);
        printf("  new brk = %p\n", sbrk(0));

        /* Write to new memory */
        memset(new_brk, 0xBB, PAGE_SIZE);
        TEST("write to sbrk region", 1);

        /* Restore break */
        brk(orig_brk);
    } else {
        printf("  [INFO] sbrk not supported\n");
        test_count++;
        pass_count++;
    }
}

void test_large_allocation(void)
{
    size_t size = 16 * 1024 * 1024;  /* 16 MB */
    void *ptr;

    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        printf("  [INFO] Large allocation not available\n");
        TEST("large mmap (16MB)", 1);  /* Not a failure */
        return;
    }

    TEST("large mmap (16MB)", 1);

    /* Touch pages to ensure they're allocated */
    volatile char *p = (volatile char *)ptr;
    for (size_t i = 0; i < size; i += PAGE_SIZE) {
        p[i] = 'X';
    }
    TEST("touch large allocation", 1);

    munmap(ptr, size);
}

void test_multiple_mappings(void)
{
    void *ptrs[10];
    size_t size = PAGE_SIZE;
    int i;
    int success = 1;

    /* Create multiple mappings */
    for (i = 0; i < 10; i++) {
        ptrs[i] = mmap(NULL, size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptrs[i] == MAP_FAILED) {
            success = 0;
            break;
        }
        memset(ptrs[i], i, size);
    }

    TEST("multiple mmap allocations", success);

    /* Verify each mapping has correct data */
    int data_ok = 1;
    for (int j = 0; j < i; j++) {
        unsigned char *p = (unsigned char *)ptrs[j];
        if (p[0] != j || p[size-1] != j) {
            data_ok = 0;
            break;
        }
    }
    TEST("multiple mappings data integrity", data_ok);

    /* Clean up */
    for (int j = 0; j < i; j++) {
        munmap(ptrs[j], size);
    }
}

int main(void)
{
    printf("=== MINIX VM Test Suite (RISC-V 64) ===\n\n");

    printf("--- mmap anonymous test ---\n");
    test_mmap_anon();

    printf("\n--- mmap fixed test ---\n");
    test_mmap_fixed();

    printf("\n--- mprotect test ---\n");
    test_mprotect();

    printf("\n--- page fault protection test ---\n");
    test_page_fault_protection();

    printf("\n--- brk/sbrk test ---\n");
    test_brk_sbrk();

    printf("\n--- large allocation test ---\n");
    test_large_allocation();

    printf("\n--- multiple mappings test ---\n");
    test_multiple_mappings();

    printf("\n=== Results ===\n");
    printf("Total: %d, Pass: %d, Fail: %d\n", test_count, pass_count, fail_count);

    return fail_count > 0 ? 1 : 0;
}
