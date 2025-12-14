#!/bin/bash
#
# QEMU launch script for MINIX/riscv64
#
# Usage:
#   ./qemu-riscv64.sh [options]
#
# Options:
#   -d          Debug mode (wait for GDB)
#   -s          Single CPU mode
#   -m SIZE     Memory size (default: 256M)
#   -k KERNEL   Kernel image path
#   -i IMAGE    Disk image path
#   -n          Enable networking
#   -g          Enable graphics (virtio-gpu)
#

set -e

# Default values
MEMORY="256M"
CPUS=4
DEBUG=0
NETWORK=0
GRAPHICS=0
KERNEL=""
DISK=""

# QEMU binary
QEMU="qemu-system-riscv64"

# Parse arguments
while getopts "dsm:k:i:ng" opt; do
    case $opt in
        d) DEBUG=1 ;;
        s) CPUS=1 ;;
        m) MEMORY="$OPTARG" ;;
        k) KERNEL="$OPTARG" ;;
        i) DISK="$OPTARG" ;;
        n) NETWORK=1 ;;
        g) GRAPHICS=1 ;;
        *) echo "Usage: $0 [-d] [-s] [-m size] [-k kernel] [-i image] [-n] [-g]"; exit 1 ;;
    esac
done

# Build QEMU command
QEMU_ARGS=(
    -machine virt
    -cpu rv64
    -smp "$CPUS"
    -m "$MEMORY"
    -nographic
)

# OpenSBI firmware (use QEMU's built-in)
QEMU_ARGS+=(-bios default)

# Kernel
if [ -n "$KERNEL" ]; then
    QEMU_ARGS+=(-kernel "$KERNEL")
else
    # Default kernel path
    KERNEL_PATH="${KERNEL_PATH:-/usr/obj/minix/riscv64/minix/kernel/kernel}"
    if [ -f "$KERNEL_PATH" ]; then
        QEMU_ARGS+=(-kernel "$KERNEL_PATH")
    fi
fi

# Disk image
if [ -n "$DISK" ]; then
    QEMU_ARGS+=(
        -drive file="$DISK",format=raw,id=hd0
        -device virtio-blk-device,drive=hd0
    )
fi

# Serial console
QEMU_ARGS+=(
    -serial mon:stdio
)

# Network
if [ "$NETWORK" -eq 1 ]; then
    QEMU_ARGS+=(
        -netdev user,id=net0,hostfwd=tcp::2222-:22
        -device virtio-net-device,netdev=net0
    )
fi

# Graphics
if [ "$GRAPHICS" -eq 1 ]; then
    # Remove -nographic, add display
    QEMU_ARGS=("${QEMU_ARGS[@]/-nographic/}")
    QEMU_ARGS+=(
        -device virtio-gpu-device
        -device virtio-keyboard-device
        -device virtio-mouse-device
    )
fi

# Debug mode
if [ "$DEBUG" -eq 1 ]; then
    QEMU_ARGS+=(
        -S
        -gdb tcp::1234
    )
    echo "Waiting for GDB connection on port 1234..."
    echo "Connect with: riscv64-unknown-elf-gdb -ex 'target remote :1234'"
fi

# RNG device
QEMU_ARGS+=(
    -device virtio-rng-device
)

echo "Starting QEMU with: $QEMU ${QEMU_ARGS[*]}"
exec "$QEMU" "${QEMU_ARGS[@]}"
