#!/bin/bash
#
# Build validation script for MINIX RISC-V 64-bit
# Validates that all critical components can be built
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Track progress
PASS=0
FAIL=0
SKIP=0

log_pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
    ((PASS++))
}

log_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
    ((FAIL++))
}

log_skip() {
    echo -e "${YELLOW}[SKIP]${NC} $1"
    ((SKIP++))
}

log_info() {
    echo -e "[INFO] $1"
}

# Validate prerequisites
validate_prereqs() {
    log_info "Validating prerequisites..."

    # Check for RISC-V cross compiler
    if command -v riscv64-unknown-elf-gcc >/dev/null 2>&1; then
        log_pass "RISC-V cross compiler found"
    elif command -v riscv64-linux-gnu-gcc >/dev/null 2>&1; then
        log_pass "RISC-V cross compiler found (Linux variant)"
        export CC=riscv64-linux-gnu-gcc
    else
        log_fail "RISC-V cross compiler not found"
        log_info "Install with: sudo apt-get install gcc-riscv64-unknown-elf"
    fi

    # Check for QEMU
    if command -v qemu-system-riscv64 >/dev/null 2>&1; then
        log_pass "QEMU RISC-V 64-bit found"
    else
        log_fail "QEMU RISC-V 64-bit not found"
        log_info "Install with: sudo apt-get install qemu-system-riscv64"
    fi
}

# Validate architecture files
validate_arch_files() {
    log_info "Validating architecture files..."

    local files=(
        "minix/kernel/arch/riscv64/head.S"
        "minix/kernel/arch/riscv64/exception.c"
        "minix/kernel/arch/riscv64/sbi.c"
        "minix/kernel/arch/riscv64/plic.c"
        "minix/kernel/arch/riscv64/memory.c"
        "minix/kernel/arch/riscv64/console.c"
        "minix/drivers/tty/ns16550/ns16550.c"
        "minix/lib/libminc/arch/riscv64/Makefile.libc.inc"
        "minix/tests/arch/riscv64/Makefile.inc"
    )

    for file in "${files[@]}"; do
        if [ -f "$file" ]; then
            log_pass "Found $file"
        else
            log_fail "Missing $file"
        fi
    done
}

# Validate kernel compilation
validate_kernel() {
    log_info "Validating kernel compilation..."

    # Test compile a single kernel file
    cd minix/kernel/arch/riscv64

    if ${CC:-riscv64-unknown-elf-gcc} -march=rv64gc -mabi=lp64d \
        -I../../../include -Iinclude -nostdinc -fno-builtin \
        -ffreestanding -c console.c -o /tmp/console_test.o 2>/dev/null; then
        log_pass "Kernel console.c compiles"
        rm -f /tmp/console_test.o
    else
        log_fail "Kernel console.c compilation failed"
    fi

    cd - >/dev/null
}

# Validate driver compilation
validate_drivers() {
    log_info "Validating driver compilation..."

    # Test compile NS16550 driver
    cd minix/drivers/tty/ns16550

    if ${CC:-riscv64-unknown-elf-gcc} -march=rv64gc -mabi=lp64d \
        -I../../../include -I../../.. -nostdinc -fno-builtin \
        -ffreestanding -c ns16550.c -o /tmp/ns16550_test.o 2>/dev/null; then
        log_pass "NS16550 driver compiles"
        rm -f /tmp/ns16550_test.o
    else
        log_fail "NS16550 driver compilation failed"
    fi

    cd - >/dev/null
}

# Run quick architecture test
validate_arch_tests() {
    log_info "Running architecture tests..."

    if [ -f "minix/tests/riscv64/run_tests.sh" ]; then
        cd minix/tests/riscv64

        # Run build tests only (no QEMU)
        if bash run_tests.sh build >/dev/null 2>&1; then
            log_pass "Architecture build tests pass"
        else
            log_fail "Architecture build tests failed"
        fi

        cd - >/dev/null
    else
        log_skip "Architecture test runner not found"
    fi
}

# Main validation
main() {
    echo "========================================="
    echo "MINIX RISC-V 64-bit Build Validation"
    echo "========================================="
    echo ""

    validate_prereqs
    echo ""

    validate_arch_files
    echo ""

    validate_kernel
    echo ""

    validate_drivers
    echo ""

    validate_arch_tests
    echo ""

    echo "========================================="
    echo "Validation Summary"
    echo "========================================="
    echo -e "Passed:  ${GREEN}${PASS}${NC}"
    echo -e "Failed:  ${RED}${FAIL}${NC}"
    echo -e "Skipped: ${YELLOW}${SKIP}${NC}"
    echo "========================================="

    if [ $FAIL -eq 0 ]; then
        log_info "Build validation PASSED"
        exit 0
    else
        log_fail "Build validation FAILED"
        exit 1
    fi
}

# Run main function
main "$@"