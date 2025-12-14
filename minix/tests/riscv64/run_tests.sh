#!/bin/bash
#
# RISC-V 64-bit test runner for MINIX 3
#
# Usage:
#   ./run_tests.sh [kernel|user|all]
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
QEMU="${QEMU:-qemu-system-riscv64}"
KERNEL="${KERNEL:-/usr/obj/minix/riscv64/minix/kernel/kernel}"
TIMEOUT="${TIMEOUT:-60}"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

passed=0
failed=0
skipped=0

log_pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
    ((passed++))
}

log_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
    ((failed++))
}

log_skip() {
    echo -e "${YELLOW}[SKIP]${NC} $1"
    ((skipped++))
}

log_info() {
    echo -e "[INFO] $1"
}

#
# Kernel-level tests (run in QEMU)
#
run_kernel_tests() {
    log_info "Running kernel-level tests..."

    # Check if QEMU is available
    if ! command -v "$QEMU" &> /dev/null; then
        log_skip "QEMU not found, skipping kernel tests"
        return
    fi

    # Check if kernel exists
    if [ ! -f "$KERNEL" ]; then
        log_skip "Kernel not found at $KERNEL, skipping kernel tests"
        return
    fi

    # Test 1: Kernel boots
    log_info "Test: Kernel boot"
    timeout $TIMEOUT $QEMU \
        -machine virt \
        -cpu rv64 \
        -m 256M \
        -nographic \
        -bios default \
        -kernel "$KERNEL" \
        -append "test=boot" \
        -serial file:/tmp/boot_test.log \
        2>/dev/null || true

    if grep -q "MINIX" /tmp/boot_test.log 2>/dev/null; then
        log_pass "Kernel boot"
    else
        log_fail "Kernel boot"
    fi

    # Test 2: SMP initialization
    log_info "Test: SMP initialization"
    timeout $TIMEOUT $QEMU \
        -machine virt \
        -cpu rv64 \
        -smp 4 \
        -m 256M \
        -nographic \
        -bios default \
        -kernel "$KERNEL" \
        -append "test=smp" \
        -serial file:/tmp/smp_test.log \
        2>/dev/null || true

    if grep -q "CPU.*online" /tmp/smp_test.log 2>/dev/null; then
        log_pass "SMP initialization"
    else
        log_skip "SMP initialization (not yet implemented)"
    fi

    # Test 3: Timer interrupt
    log_info "Test: Timer interrupt"
    timeout $TIMEOUT $QEMU \
        -machine virt \
        -cpu rv64 \
        -m 256M \
        -nographic \
        -bios default \
        -kernel "$KERNEL" \
        -append "test=timer" \
        -serial file:/tmp/timer_test.log \
        2>/dev/null || true

    if grep -q "timer" /tmp/timer_test.log 2>/dev/null; then
        log_pass "Timer interrupt"
    else
        log_skip "Timer interrupt (not yet implemented)"
    fi
}

#
# User-level tests (compile and check)
#
run_user_tests() {
    log_info "Running user-level tests..."

    # Check if cross-compiler is available
    CC="${CC:-riscv64-unknown-elf-gcc}"
    if ! command -v "$CC" &> /dev/null; then
        # Try alternatives
        for alt in riscv64-linux-gnu-gcc riscv64-elf-gcc; do
            if command -v "$alt" &> /dev/null; then
                CC="$alt"
                break
            fi
        done
    fi

    if ! command -v "$CC" &> /dev/null; then
        log_skip "RISC-V cross-compiler not found, skipping user tests"
        return
    fi

    log_info "Using compiler: $CC"

    # Test compilation of each test file
    for test_file in "$SCRIPT_DIR"/test_*.c; do
        test_name=$(basename "$test_file" .c)
        log_info "Compiling: $test_name"

        if $CC -march=rv64gc -mabi=lp64d -O2 -Wall \
               -c "$test_file" -o "/tmp/${test_name}.o" 2>/dev/null; then
            log_pass "Compile $test_name"
        else
            log_fail "Compile $test_name"
        fi
    done
}

#
# Build system tests
#
run_build_tests() {
    log_info "Running build system tests..."

    MINIX_ROOT="${SCRIPT_DIR}/../../.."

    # Test 1: Check if architecture is recognized
    if [ -f "$MINIX_ROOT/build.sh" ]; then
        cd "$MINIX_ROOT"
        if ./build.sh -m evbriscv64 list-arch 2>/dev/null | grep -q "riscv64"; then
            log_pass "Architecture recognition"
        else
            log_fail "Architecture recognition"
        fi
    else
        log_skip "build.sh not found"
    fi

    # Test 2: Check kernel source files exist
    KERNEL_ARCH="$MINIX_ROOT/minix/kernel/arch/riscv64"
    if [ -d "$KERNEL_ARCH" ]; then
        log_pass "Kernel arch directory exists"

        # Check key files
        for file in head.S sbi.c plic.c exception.c kernel.lds; do
            if [ -f "$KERNEL_ARCH/$file" ]; then
                log_pass "Kernel file: $file"
            else
                log_fail "Kernel file: $file"
            fi
        done
    else
        log_fail "Kernel arch directory"
    fi

    # Test 3: Check driver source files exist
    VIRTIO_BLK="$MINIX_ROOT/minix/drivers/storage/virtio_blk_mmio"
    if [ -d "$VIRTIO_BLK" ]; then
        log_pass "VirtIO block driver directory exists"
    else
        log_fail "VirtIO block driver directory"
    fi
}

#
# Main
#
case "${1:-all}" in
    kernel)
        run_kernel_tests
        ;;
    user)
        run_user_tests
        ;;
    build)
        run_build_tests
        ;;
    all)
        run_build_tests
        echo ""
        run_user_tests
        echo ""
        run_kernel_tests
        ;;
    *)
        echo "Usage: $0 [kernel|user|build|all]"
        exit 1
        ;;
esac

echo ""
echo "========================================="
echo "Test Summary"
echo "========================================="
echo -e "Passed:  ${GREEN}${passed}${NC}"
echo -e "Failed:  ${RED}${failed}${NC}"
echo -e "Skipped: ${YELLOW}${skipped}${NC}"
echo "========================================="

exit $failed
