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
#   -B DIR      Boot modules root (destdir) to load into memory
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
BOOTMODS=0
MODROOT=""

# Paths
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MINIX_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
OBJDIR="${OBJDIR:-$MINIX_ROOT/../obj}"

# Boot module layout (must match kernel constants)
MODULE_BASE_ADDR="${MODULE_BASE_ADDR:-0x81000000}"
MODINFO_ADDR="${MODINFO_ADDR:-0x80f00000}"
MODINFO_MAGIC="${MODINFO_MAGIC:-0x584e494d}"
MODULE_ALIGN="${MODULE_ALIGN:-0x1000}"

# QEMU binary
QEMU="qemu-system-riscv64"

# Parse arguments
while getopts "dsm:k:i:B:ng" opt; do
    case $opt in
        d) DEBUG=1 ;;
        s) CPUS=1 ;;
        m) MEMORY="$OPTARG" ;;
        k) KERNEL="$OPTARG" ;;
        i) DISK="$OPTARG" ;;
        B) BOOTMODS=1; MODROOT="$OPTARG" ;;
        n) NETWORK=1 ;;
        g) GRAPHICS=1 ;;
        *) echo "Usage: $0 [-d] [-s] [-m size] [-k kernel] [-i image] [-B destdir] [-n] [-g]"; exit 1 ;;
    esac
done

find_boot_module() {
    local name=$1
    shift
    local path

    for path in "$@"; do
        if [ -f "$MODROOT/$path" ]; then
            echo "$MODROOT/$path"
            return 0
        fi
    done

    echo "Boot module not found: $name" >&2
    return 1
}

add_boot_modules() {
    local addr
    local modinfo_file
    local module_path
    local module_size
    local module_paths=()
    local module_addrs=()
    local module_sizes=()

    if [ -z "$MODROOT" ]; then
        MODROOT="$OBJDIR/destdir.evbriscv64"
    fi
    if [ ! -d "$MODROOT" ]; then
        echo "Boot modules root not found: $MODROOT" >&2
        exit 1
    fi
    if ! command -v python3 >/dev/null 2>&1; then
        echo "python3 is required to build boot module info" >&2
        exit 1
    fi

    module_path=$(find_boot_module "ds" "service/ds" "minix/servers/ds/ds") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "rs" "service/rs" "minix/servers/rs/rs") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "pm" "service/pm" "minix/servers/pm/pm") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "sched" "service/sched" "minix/servers/sched/sched") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "vfs" "service/vfs" "minix/servers/vfs/vfs") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "memory" \
        "service/memory" \
        "minix/drivers/storage/memory/memory" \
        "minix/drivers/system/memory/memory") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "tty" "service/tty" "minix/drivers/tty/tty/tty") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "mib" "service/mib" "minix/servers/mib/mib") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "vm" "service/vm" "minix/servers/vm/vm") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "pfs" "service/pfs" "minix/fs/pfs/pfs") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "mfs" "service/mfs" "minix/fs/mfs/mfs") || exit 1
    module_paths+=("$module_path")
    module_path=$(find_boot_module "init" "sbin/init" "sbin/init/init") || exit 1
    module_paths+=("$module_path")

    addr=$((MODULE_BASE_ADDR))
    for module_path in "${module_paths[@]}"; do
        module_size=$(stat -c%s "$module_path")
        addr=$(( (addr + MODULE_ALIGN - 1) & ~(MODULE_ALIGN - 1) ))
        if (( addr > 0xffffffff )); then
            echo "Module address above 4GB: $module_path" >&2
            exit 1
        fi
        module_addrs+=("$addr")
        module_sizes+=("$module_size")
        addr=$((addr + module_size))
    done

    modinfo_args=()
    for i in "${!module_paths[@]}"; do
        modinfo_args+=("${module_addrs[$i]}" "${module_sizes[$i]}")
    done

    modinfo_file=$(mktemp "${TMPDIR:-/tmp}/minix-rv64-modinfo.XXXXXX")
    python3 - "$modinfo_file" "$MODINFO_MAGIC" "${#module_paths[@]}" \
        "${modinfo_args[@]}" << 'PY'
import struct
import sys

out = sys.argv[1]
magic = int(sys.argv[2], 0)
count = int(sys.argv[3], 0)
values = sys.argv[4:]

if len(values) != count * 2:
    raise SystemExit("boot module argument mismatch")

mods = []
for i in range(count):
    start = int(values[i * 2], 0)
    size = int(values[i * 2 + 1], 0)
    end = start + size
    if end > 0xffffffff:
        raise SystemExit("module end above 4GB")
    mods.append((start, end))

with open(out, "wb") as f:
    f.write(struct.pack("<II", magic, count))
    for start, end in mods:
        f.write(struct.pack("<IIII", start, end, 0, 0))
PY

    printf "Boot modules (%d):\n" "${#module_paths[@]}"
    for i in "${!module_paths[@]}"; do
        printf -v addr_hex "0x%x" "${module_addrs[$i]}"
        printf "  %s @ %s\n" "${module_paths[$i]}" "$addr_hex"
    done

    QEMU_ARGS+=(
        -device loader,file="$modinfo_file",addr="$MODINFO_ADDR",force-raw=on
    )
    for i in "${!module_paths[@]}"; do
        printf -v addr_hex "0x%x" "${module_addrs[$i]}"
        QEMU_ARGS+=(
            -device loader,file="${module_paths[$i]}",addr="$addr_hex",force-raw=on
        )
    done
}

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

# Boot modules
if [ "$BOOTMODS" -eq 1 ]; then
    add_boot_modules
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
