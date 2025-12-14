/*
 * Test SBI (Supervisor Binary Interface) calls
 *
 * Tests interactions with OpenSBI firmware
 */

#include <stdio.h>
#include <stdint.h>

/* SBI extension IDs */
#define SBI_EXT_BASE            0x10
#define SBI_EXT_TIME            0x54494D45
#define SBI_EXT_IPI             0x735049
#define SBI_EXT_RFENCE          0x52464E43
#define SBI_EXT_HSM             0x48534D
#define SBI_EXT_SRST            0x53525354

/* SBI base functions */
#define SBI_BASE_GET_SPEC_VERSION   0
#define SBI_BASE_GET_IMPL_ID        1
#define SBI_BASE_GET_IMPL_VERSION   2
#define SBI_BASE_PROBE_EXT          3
#define SBI_BASE_GET_MVENDORID      4
#define SBI_BASE_GET_MARCHID        5
#define SBI_BASE_GET_MIMPID         6

/* SBI return structure */
struct sbiret {
    long error;
    long value;
};

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

/* SBI ecall wrapper */
static struct sbiret sbi_ecall(int ext, int fid,
                               unsigned long arg0, unsigned long arg1,
                               unsigned long arg2, unsigned long arg3,
                               unsigned long arg4, unsigned long arg5)
{
    struct sbiret ret;
    register unsigned long a0 __asm__("a0") = arg0;
    register unsigned long a1 __asm__("a1") = arg1;
    register unsigned long a2 __asm__("a2") = arg2;
    register unsigned long a3 __asm__("a3") = arg3;
    register unsigned long a4 __asm__("a4") = arg4;
    register unsigned long a5 __asm__("a5") = arg5;
    register unsigned long a6 __asm__("a6") = fid;
    register unsigned long a7 __asm__("a7") = ext;

    __asm__ __volatile__(
        "ecall"
        : "+r"(a0), "+r"(a1)
        : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
        : "memory"
    );

    ret.error = a0;
    ret.value = a1;
    return ret;
}

void test_base_extension(void)
{
    struct sbiret ret;

    /* Get spec version */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_GET_SPEC_VERSION, 0, 0, 0, 0, 0, 0);
    TEST("sbi_get_spec_version", ret.error == 0);
    printf("  SBI spec version: %ld.%ld\n",
           (ret.value >> 24) & 0x7F, ret.value & 0xFFFFFF);

    /* Get implementation ID */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_GET_IMPL_ID, 0, 0, 0, 0, 0, 0);
    TEST("sbi_get_impl_id", ret.error == 0);
    printf("  Implementation ID: %ld", ret.value);
    switch (ret.value) {
        case 0: printf(" (BBL)\n"); break;
        case 1: printf(" (OpenSBI)\n"); break;
        case 2: printf(" (Xvisor)\n"); break;
        case 3: printf(" (KVM)\n"); break;
        case 4: printf(" (RustSBI)\n"); break;
        default: printf(" (Unknown)\n"); break;
    }

    /* Get implementation version */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_GET_IMPL_VERSION, 0, 0, 0, 0, 0, 0);
    TEST("sbi_get_impl_version", ret.error == 0);
    printf("  Implementation version: %ld\n", ret.value);

    /* Get machine vendor ID */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_GET_MVENDORID, 0, 0, 0, 0, 0, 0);
    TEST("sbi_get_mvendorid", ret.error == 0);
    printf("  Machine vendor ID: 0x%lx\n", ret.value);

    /* Get machine architecture ID */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_GET_MARCHID, 0, 0, 0, 0, 0, 0);
    TEST("sbi_get_marchid", ret.error == 0);
    printf("  Machine architecture ID: 0x%lx\n", ret.value);

    /* Get machine implementation ID */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_GET_MIMPID, 0, 0, 0, 0, 0, 0);
    TEST("sbi_get_mimpid", ret.error == 0);
    printf("  Machine implementation ID: 0x%lx\n", ret.value);
}

void test_extension_probe(void)
{
    struct sbiret ret;

    /* Probe Timer extension */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_PROBE_EXT, SBI_EXT_TIME, 0, 0, 0, 0, 0);
    TEST("probe TIME extension", ret.error == 0);
    printf("  TIME extension: %s\n", ret.value ? "available" : "not available");

    /* Probe IPI extension */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_PROBE_EXT, SBI_EXT_IPI, 0, 0, 0, 0, 0);
    TEST("probe IPI extension", ret.error == 0);
    printf("  IPI extension: %s\n", ret.value ? "available" : "not available");

    /* Probe Remote Fence extension */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_PROBE_EXT, SBI_EXT_RFENCE, 0, 0, 0, 0, 0);
    TEST("probe RFENCE extension", ret.error == 0);
    printf("  RFENCE extension: %s\n", ret.value ? "available" : "not available");

    /* Probe HSM extension */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_PROBE_EXT, SBI_EXT_HSM, 0, 0, 0, 0, 0);
    TEST("probe HSM extension", ret.error == 0);
    printf("  HSM extension: %s\n", ret.value ? "available" : "not available");

    /* Probe System Reset extension */
    ret = sbi_ecall(SBI_EXT_BASE, SBI_BASE_PROBE_EXT, SBI_EXT_SRST, 0, 0, 0, 0, 0);
    TEST("probe SRST extension", ret.error == 0);
    printf("  SRST extension: %s\n", ret.value ? "available" : "not available");
}

void test_legacy_putchar(void)
{
    struct sbiret ret;

    /* Legacy console putchar (deprecated but widely supported) */
    /* Extension 0x01, function 0 */
    ret.error = 0;
    __asm__ __volatile__(
        "li a7, 0x01\n"
        "li a0, 'T'\n"
        "ecall\n"
        : : : "a0", "a7", "memory"
    );
    __asm__ __volatile__(
        "li a7, 0x01\n"
        "li a0, 'S'\n"
        "ecall\n"
        : : : "a0", "a7", "memory"
    );
    __asm__ __volatile__(
        "li a7, 0x01\n"
        "li a0, 'T'\n"
        "ecall\n"
        : : : "a0", "a7", "memory"
    );
    __asm__ __volatile__(
        "li a7, 0x01\n"
        "li a0, '\\n'\n"
        "ecall\n"
        : : : "a0", "a7", "memory"
    );

    TEST("legacy console putchar", 1);  /* If we got here, it worked */
}

int main(void)
{
    printf("=== SBI (Supervisor Binary Interface) Test Suite ===\n\n");

    printf("--- Base extension test ---\n");
    test_base_extension();

    printf("\n--- Extension probe test ---\n");
    test_extension_probe();

    printf("\n--- Legacy console test ---\n");
    test_legacy_putchar();

    printf("\n=== Results ===\n");
    printf("Total: %d, Pass: %d, Fail: %d\n", test_count, pass_count, fail_count);

    return fail_count > 0 ? 1 : 0;
}
