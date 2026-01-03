/*
 * Test RISC-V trap handling
 *
 * Tests exception and interrupt handling
 */

#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>

#define STR(x) #x
#define XSTR(x) STR(x)

#define CSR_STVEC   0x105
#define CSR_SCAUSE  0x142
#define CSR_STVAL   0x143

#define RISCV_EBREAK_INSN 0x00100073
#define RISCV_WFI_INSN    0x10500073

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

/* Exception causes (scause values) */
#define CAUSE_MISALIGNED_FETCH      0
#define CAUSE_FETCH_ACCESS          1
#define CAUSE_ILLEGAL_INSTRUCTION   2
#define CAUSE_BREAKPOINT            3
#define CAUSE_MISALIGNED_LOAD       4
#define CAUSE_LOAD_ACCESS           5
#define CAUSE_MISALIGNED_STORE      6
#define CAUSE_STORE_ACCESS          7
#define CAUSE_USER_ECALL            8
#define CAUSE_SUPERVISOR_ECALL      9
#define CAUSE_MACHINE_ECALL         11
#define CAUSE_FETCH_PAGE_FAULT      12
#define CAUSE_LOAD_PAGE_FAULT       13
#define CAUSE_STORE_PAGE_FAULT      15

/* Signal handler flag */
static volatile sig_atomic_t got_signal = 0;
static volatile int signal_code = 0;
static jmp_buf jump_buf;

void signal_handler(int sig)
{
    got_signal = 1;
    signal_code = sig;
    longjmp(jump_buf, 1);
}

void test_illegal_instruction(void)
{
    /*
     * Test illegal instruction handling
     * Note: This will cause SIGILL if signal handling works
     */
    got_signal = 0;

    struct sigaction sa, old_sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGILL, &sa, &old_sa) < 0) {
        printf("  [INFO] Cannot install signal handler, skipping\n");
        return;
    }

    if (setjmp(jump_buf) == 0) {
        /* Try to execute illegal instruction */
        /* 0x00000000 is an illegal instruction in RISC-V */
        __asm__ __volatile__(".word 0x00000000" ::: "memory");
    }

    sigaction(SIGILL, &old_sa, NULL);

    TEST("illegal instruction generates SIGILL", got_signal && signal_code == SIGILL);
}

void test_breakpoint(void)
{
    /*
     * Test breakpoint instruction (EBREAK)
     * Note: This will cause SIGTRAP if signal handling works
     */
    got_signal = 0;

    struct sigaction sa, old_sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTRAP, &sa, &old_sa) < 0) {
        printf("  [INFO] Cannot install signal handler, skipping\n");
        return;
    }

    if (setjmp(jump_buf) == 0) {
        /* Execute EBREAK */
        __asm__ __volatile__(".word " XSTR(RISCV_EBREAK_INSN) ::: "memory");
    }

    sigaction(SIGTRAP, &old_sa, NULL);

    TEST("EBREAK generates SIGTRAP", got_signal && signal_code == SIGTRAP);
}

void test_ecall(void)
{
    /*
     * Test system call (ECALL)
     * A simple syscall should work without trapping to signal handler
     */
    long result;

    /* getpid syscall */
    __asm__ __volatile__(
        "li a7, 20\n"   /* SYS_getpid */
        "ecall\n"
        "mv %0, a0\n"
        : "=r"(result)
        :
        : "a0", "a7", "memory"
    );

    TEST("ECALL (getpid) returns valid pid", result > 0);
    printf("  pid = %ld\n", result);
}

void test_csrs(void)
{
    /*
     * Test reading exception-related CSRs
     */
    uint64_t stvec, scause, stval;

    /* Read stvec (trap vector base) */
    __asm__ __volatile__("csrr %0, " XSTR(CSR_STVEC) : "=r"(stvec));
    printf("  stvec = 0x%lx\n", stvec);
    printf("  MODE = %s\n", (stvec & 3) == 0 ? "Direct" : "Vectored");
    TEST("stvec readable", 1);

    /* scause and stval are only valid after a trap */
    __asm__ __volatile__("csrr %0, " XSTR(CSR_SCAUSE) : "=r"(scause));
    __asm__ __volatile__("csrr %0, " XSTR(CSR_STVAL) : "=r"(stval));
    printf("  scause = 0x%lx (last trap cause)\n", scause);
    printf("  stval = 0x%lx (last trap value)\n", stval);
    TEST("scause/stval readable", 1);
}

void test_wfi(void)
{
    /*
     * Test WFI (Wait For Interrupt)
     * Should return immediately if any interrupt is pending
     * or after a short timeout in QEMU
     */

    /* WFI might trap to M-mode depending on mstatus.TW */
    /* In user mode, it might be emulated as NOP */
    __asm__ __volatile__(".word " XSTR(RISCV_WFI_INSN) ::: "memory");

    TEST("WFI instruction", 1);  /* If we get here, it worked */
}

void test_fence_i(void)
{
    /*
     * Test FENCE.I (instruction fence)
     * Used for self-modifying code synchronization
     */
    __asm__ __volatile__("fence.i" ::: "memory");
    TEST("FENCE.I instruction", 1);
}

int main(void)
{
    printf("=== RISC-V Trap Handling Test Suite ===\n\n");

    printf("--- CSR test ---\n");
    test_csrs();

    printf("\n--- ECALL test ---\n");
    test_ecall();

    printf("\n--- WFI test ---\n");
    test_wfi();

    printf("\n--- FENCE.I test ---\n");
    test_fence_i();

    printf("\n--- Illegal instruction test ---\n");
    test_illegal_instruction();

    printf("\n--- Breakpoint test ---\n");
    test_breakpoint();

    printf("\n=== Results ===\n");
    printf("Total: %d, Pass: %d, Fail: %d\n", test_count, pass_count, fail_count);

    return fail_count > 0 ? 1 : 0;
}
