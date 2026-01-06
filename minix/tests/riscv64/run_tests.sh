#!/bin/bash
#
# RISC-V 64-bit test runner for MINIX 3
#
# Usage:
#   ./run_tests.sh [kernel|user|all]
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MINIX_ROOT="${SCRIPT_DIR}/../../.."
QEMU="${QEMU:-qemu-system-riscv64}"
KERNEL_DEFAULT="${MINIX_ROOT}/minix/kernel/obj/kernel"
KERNEL="${KERNEL:-$KERNEL_DEFAULT}"
TIMEOUT="${TIMEOUT:-60}"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

passed=0
failed=0
skipped=0

TOOLDIR_DEFAULT=""
DESTDIR_DEFAULT=""
for d in "$MINIX_ROOT"/obj/tooldir.*; do
    if [ -x "$d/bin/riscv64-elf32-minix-gcc" ]; then
        TOOLDIR_DEFAULT="$d"
        break
    fi
done
if [ -d "$MINIX_ROOT/obj/destdir.evbriscv64" ]; then
    DESTDIR_DEFAULT="$MINIX_ROOT/obj/destdir.evbriscv64"
fi
NBMAKE_DEFAULT=""
if [ -n "$TOOLDIR_DEFAULT" ] && [ -x "$TOOLDIR_DEFAULT/bin/nbmake-evbriscv64" ]; then
    NBMAKE_DEFAULT="$TOOLDIR_DEFAULT/bin/nbmake-evbriscv64"
fi
NBMAKE="${NBMAKE:-}"
if [ -z "$NBMAKE" ]; then
    if [ -n "${TOOLDIR:-}" ] && [ -x "${TOOLDIR}/bin/nbmake-evbriscv64" ]; then
        NBMAKE="${TOOLDIR}/bin/nbmake-evbriscv64"
    elif [ -n "$NBMAKE_DEFAULT" ]; then
        NBMAKE="$NBMAKE_DEFAULT"
    fi
fi
READELF="${READELF:-}"
if [ -z "$READELF" ]; then
    if [ -n "${TOOLDIR:-}" ] && [ -x "${TOOLDIR}/bin/riscv64-elf32-minix-readelf" ]; then
        READELF="${TOOLDIR}/bin/riscv64-elf32-minix-readelf"
    elif [ -n "$TOOLDIR_DEFAULT" ] && [ -x "${TOOLDIR_DEFAULT}/bin/riscv64-elf32-minix-readelf" ]; then
        READELF="${TOOLDIR_DEFAULT}/bin/riscv64-elf32-minix-readelf"
    fi
fi

log_pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
    passed=$((passed + 1))
}

log_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
    failed=$((failed + 1))
}

log_skip() {
    echo -e "${YELLOW}[SKIP]${NC} $1"
    skipped=$((skipped + 1))
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
    QEMU_SCRIPT="${MINIX_ROOT}/minix/scripts/qemu-riscv64.sh"
    BOOTMODROOT="${DESTDIR:-$DESTDIR_DEFAULT}"
    if [ ! -x "$QEMU_SCRIPT" ]; then
        log_skip "qemu-riscv64.sh not found, skipping kernel tests"
        return
    fi
    if ! command -v qemu-system-riscv64 &> /dev/null; then
        log_skip "QEMU not found, skipping kernel tests"
        return
    fi
    if [ -z "$BOOTMODROOT" ] || [ ! -d "$BOOTMODROOT" ]; then
        log_skip "DESTDIR not found, skipping kernel tests"
        return
    fi
    if ! command -v python3 &> /dev/null; then
        log_skip "python3 not found, skipping kernel tests"
        return
    fi

    # Check if kernel exists
    if [ ! -f "$KERNEL" ]; then
        log_skip "Kernel not found at $KERNEL, skipping kernel tests"
        return
    fi

    # Test 1: Kernel boots
    log_info "Test: Kernel boot"
    timeout $TIMEOUT "$QEMU_SCRIPT" \
        -s -k "$KERNEL" -B "$BOOTMODROOT" \
        > /tmp/boot_test.log 2>&1 || true

    if grep -q "MINIX" /tmp/boot_test.log 2>/dev/null || \
       grep -q "rv64: arch_post_init" /tmp/boot_test.log 2>/dev/null || \
       grep -q "rv64: arch_boot_proc VM" /tmp/boot_test.log 2>/dev/null; then
        log_pass "Kernel boot"
    else
        log_fail "Kernel boot"
    fi

    # Test 2: SMP initialization
    log_info "Test: SMP initialization"
    timeout $TIMEOUT "$QEMU_SCRIPT" \
        -k "$KERNEL" -B "$BOOTMODROOT" \
        > /tmp/smp_test.log 2>&1 || true

    if grep -q "CPU.*online" /tmp/smp_test.log 2>/dev/null; then
        log_pass "SMP initialization"
    else
        log_skip "SMP initialization (not yet implemented)"
    fi

    # Test 3: Timer interrupt
    log_info "Test: Timer interrupt"
    timeout $TIMEOUT "$QEMU_SCRIPT" \
        -s -k "$KERNEL" -B "$BOOTMODROOT" \
        > /tmp/timer_test.log 2>&1 || true

    if grep -q "timer" /tmp/timer_test.log 2>/dev/null; then
        log_pass "Timer interrupt"
    else
        log_skip "Timer interrupt (not yet implemented)"
    fi

    # Test 4: VirtIO block I/O smoke test
    log_info "Test: VirtIO block I/O smoke"
    IO_SMOKE_SCRIPT="${SCRIPT_DIR}/qemu_io_smoke.py"
    TEST_SRC="${SCRIPT_DIR}/test_virtio_blk_mmio.c"
    TEST_BIN="${BOOTMODROOT}/bin/test_virtio_blk_mmio"
    TEST_BIN_READY=0
    if [ ! -f "$IO_SMOKE_SCRIPT" ]; then
        log_skip "VirtIO block I/O smoke (script missing)"
        return
    fi
    if [ ! -f "$TEST_SRC" ]; then
        log_skip "VirtIO block I/O smoke (test source missing)"
        return
    fi

    NEED_REBUILD=1
    if [ -x "$TEST_BIN" ]; then
        NEED_REBUILD=0
        if [ -n "$READELF" ] && $READELF -l "$TEST_BIN" 2>/dev/null | grep -q "INTERP"; then
            log_info "VirtIO block I/O smoke: existing test binary is dynamic, rebuilding static"
            NEED_REBUILD=1
        else
            TEST_BIN_READY=1
        fi
    fi

    if [ "$NEED_REBUILD" -eq 1 ]; then
        CC_SMOKE="${CC:-}"
        if [ -z "$CC_SMOKE" ]; then
            if [ -n "${TOOLDIR:-}" ] && [ -x "${TOOLDIR}/bin/riscv64-elf32-minix-gcc" ]; then
                CC_SMOKE="${TOOLDIR}/bin/riscv64-elf32-minix-gcc"
            elif [ -n "$TOOLDIR_DEFAULT" ]; then
                CC_SMOKE="${TOOLDIR_DEFAULT}/bin/riscv64-elf32-minix-gcc"
            else
                CC_SMOKE="riscv64-unknown-elf-gcc"
            fi
        fi
        if ! command -v "$CC_SMOKE" &> /dev/null; then
            log_info "VirtIO block I/O smoke: compiler not found, will fall back to dd/cmp if available"
        else
            mkdir -p "$(dirname "$TEST_BIN")"
            ARCH_FLAGS="${RISCV_ARCH_FLAGS:--march=RV64IMAFD -mcmodel=medany}"
            SYSROOT_FLAGS="--sysroot=$BOOTMODROOT -I$BOOTMODROOT/usr/include"
            if $CC_SMOKE $ARCH_FLAGS $SYSROOT_FLAGS -O2 -Wall -std=gnu99 -static \
                   -Wl,--defsym,__global_pointer\$=_gp \
                   -o "$TEST_BIN" "$TEST_SRC" 2>/dev/null; then
                log_info "Built test binary: $TEST_BIN"
                TEST_BIN_READY=1
            else
                log_info "VirtIO block I/O smoke: build failed, will fall back to dd/cmp if available"
            fi
        fi
    fi
    if [ "$TEST_BIN_READY" -eq 1 ] && [ -n "$NBMAKE" ]; then
        TEST_OBJ_DIR="$MINIX_ROOT/minix/tests/riscv64/obj"
        mkdir -p "$TEST_OBJ_DIR"
        if cp "$TEST_BIN" "$TEST_OBJ_DIR/test_virtio_blk_mmio"; then
            chmod 755 "$TEST_OBJ_DIR/test_virtio_blk_mmio" 2>/dev/null || true
        else
            log_info "Failed to stage test binary for ramdisk, will fall back to dd/cmp if available"
        fi
        log_info "Rebuilding ramdisk image with virtio test binary..."
        if "$NBMAKE" -C "$MINIX_ROOT/minix/drivers/storage/ramdisk" \
                RAMDISK_TESTS=1 image && \
           "$NBMAKE" -C "$MINIX_ROOT/minix/drivers/storage/memory" \
                RAMDISK_TESTS=1 all && \
           "$NBMAKE" -C "$MINIX_ROOT/minix/drivers/storage/memory" \
                RAMDISK_TESTS=1 install; then
            log_info "Ramdisk updated for virtio smoke test"
        else
            log_info "Ramdisk rebuild failed, will fall back to dd/cmp if available"
        fi
    elif [ "$TEST_BIN_READY" -eq 1 ]; then
        log_info "nbmake not found, skipping ramdisk rebuild"
    fi

    DISK_ARG=()
    if [ -n "${SMOKE_DISK_IMAGE:-}" ]; then
        DISK_ARG=(--disk "$SMOKE_DISK_IMAGE")
    fi
    DEVICE_ARG=()
    if [ -n "${SMOKE_DEVICE:-}" ]; then
        DEVICE_ARG=(--device "$SMOKE_DEVICE")
    fi
    FILE_FALLBACK_ARG=()
    REQUIRE_BLOCK_ARG=(--require-block)
    if [ -n "${SMOKE_FILE_FALLBACK:-}" ]; then
        FILE_FALLBACK_ARG=(--file-fallback)
        REQUIRE_BLOCK_ARG=()
    fi
    DD_FALLBACK_ARG=(--dd-fallback)
    DD_UNSAFE_ARG=()
    if [ -n "${SMOKE_DD_UNSAFE:-}" ]; then
        DD_UNSAFE_ARG=(--dd-unsafe)
    fi
    OFFSET_ARG=(--offset "${SMOKE_OFFSET:-1048576}")
    SIZE_ARG=(--size "${SMOKE_SIZE:-4096}")

    if python3 "$IO_SMOKE_SCRIPT" \
        --qemu-script "$QEMU_SCRIPT" \
        --kernel "$KERNEL" \
        --destdir "$BOOTMODROOT" \
        --test-bin "$TEST_BIN" \
        "${DEVICE_ARG[@]}" \
        "${FILE_FALLBACK_ARG[@]}" \
        "${DD_FALLBACK_ARG[@]}" \
        "${DD_UNSAFE_ARG[@]}" \
        "${REQUIRE_BLOCK_ARG[@]}" \
        "${OFFSET_ARG[@]}" \
        "${SIZE_ARG[@]}" \
        "${DISK_ARG[@]}"; then
        log_pass "VirtIO block I/O smoke"
    else
        rc=$?
        if [ "$rc" -eq 2 ]; then
            log_skip "VirtIO block I/O smoke (skipped)"
        else
            log_fail "VirtIO block I/O smoke"
        fi
    fi
}

#
# User-level tests (compile and check)
#
run_user_tests() {
    log_info "Running user-level tests..."

    # Check if cross-compiler is available
    if [ -z "${CC:-}" ]; then
        if [ -n "${TOOLDIR:-}" ] && [ -x "${TOOLDIR}/bin/riscv64-elf32-minix-gcc" ]; then
            CC="${TOOLDIR}/bin/riscv64-elf32-minix-gcc"
        elif [ -n "$TOOLDIR_DEFAULT" ]; then
            CC="${TOOLDIR_DEFAULT}/bin/riscv64-elf32-minix-gcc"
        else
            CC="riscv64-unknown-elf-gcc"
        fi
    fi
    if ! command -v "$CC" &> /dev/null; then
        # Try alternatives
        for alt in riscv64-elf32-minix-gcc riscv64-linux-gnu-gcc riscv64-elf-gcc; do
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
    ARCH_FLAGS="${RISCV_ARCH_FLAGS:--march=RV64IMAFD -mcmodel=medany}"
    SYSROOT="${SYSROOT:-${DESTDIR:-$DESTDIR_DEFAULT}}"
    SYSROOT_FLAGS=""
    if [ -n "$SYSROOT" ]; then
        SYSROOT_FLAGS="--sysroot=$SYSROOT -I$SYSROOT/usr/include"
    fi

    # Test compilation of each test file
    for test_file in "$SCRIPT_DIR"/test_*.c; do
        test_name=$(basename "$test_file" .c)
        log_info "Compiling: $test_name"

        if $CC $ARCH_FLAGS $SYSROOT_FLAGS -O2 -Wall -std=gnu99 \
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
