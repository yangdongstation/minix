#!/bin/bash
#
# Create bootable disk image for MINIX 3 RISC-V 64-bit
#
# Usage:
#   ./mkdisk.sh [options]
#
# Options:
#   -d DESTDIR     Destination directory
#   -o OUTPUT      Output disk image
#   -s SIZE        Disk size in MB (default: 256)
#   -h             Show help
#

set -e

# Default values
DESTDIR="${DESTDIR:-/usr/obj/minix/riscv64}"
OUTPUT="${OUTPUT:-minix-riscv64.img}"
SIZE_MB=256

# Script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

log_info() {
    echo "[INFO] $1"
}

log_error() {
    echo "[ERROR] $1" >&2
}

usage() {
    cat << EOF
Usage: $0 [options]

Create bootable disk image for MINIX 3 RISC-V 64-bit.

Options:
    -d DESTDIR     Destination directory (default: /usr/obj/minix/riscv64)
    -o OUTPUT      Output disk image (default: minix-riscv64.img)
    -s SIZE        Disk size in MB (default: 256)
    -h             Show help

Example:
    $0 -s 512 -o my-minix.img
EOF
    exit 0
}

# Parse arguments
while getopts "d:o:s:h" opt; do
    case $opt in
        d) DESTDIR="$OPTARG" ;;
        o) OUTPUT="$OPTARG" ;;
        s) SIZE_MB="$OPTARG" ;;
        h) usage ;;
        *) usage ;;
    esac
done

# Calculate partition sizes
BOOT_SIZE_MB=8
ROOT_SIZE_MB=64
USR_SIZE_MB=128
HOME_SIZE_MB=$((SIZE_MB - BOOT_SIZE_MB - ROOT_SIZE_MB - USR_SIZE_MB - 16))  # 16MB for metadata

# Create empty disk image
create_disk() {
    log_info "Creating disk image: $OUTPUT ($SIZE_MB MB)"
    dd if=/dev/zero of="$OUTPUT" bs=1M count=$SIZE_MB 2>/dev/null
}

# Create partition table
create_partitions() {
    log_info "Creating partition table..."

    # Check for parted or fdisk
    if command -v parted &> /dev/null; then
        parted -s "$OUTPUT" mklabel msdos
        parted -s "$OUTPUT" mkpart primary ext2 1MiB $((BOOT_SIZE_MB + 1))MiB
        parted -s "$OUTPUT" mkpart primary ext2 $((BOOT_SIZE_MB + 1))MiB $((BOOT_SIZE_MB + ROOT_SIZE_MB + 1))MiB
        parted -s "$OUTPUT" mkpart primary ext2 $((BOOT_SIZE_MB + ROOT_SIZE_MB + 1))MiB $((BOOT_SIZE_MB + ROOT_SIZE_MB + USR_SIZE_MB + 1))MiB
        parted -s "$OUTPUT" mkpart extended $((BOOT_SIZE_MB + ROOT_SIZE_MB + USR_SIZE_MB + 1))MiB 100%
        parted -s "$OUTPUT" mkpart logical ext2 $((BOOT_SIZE_MB + ROOT_SIZE_MB + USR_SIZE_MB + 2))MiB 100%
        parted -s "$OUTPUT" set 1 boot on
    elif command -v sfdisk &> /dev/null; then
        # Use sfdisk
        cat << EOF | sfdisk --no-reread "$OUTPUT" 2>/dev/null
label: dos
unit: sectors

${OUTPUT}p1 : start=2048, size=$((BOOT_SIZE_MB * 2048)), type=83, bootable
${OUTPUT}p2 : start=$((2048 + BOOT_SIZE_MB * 2048)), size=$((ROOT_SIZE_MB * 2048)), type=83
${OUTPUT}p3 : start=$((2048 + (BOOT_SIZE_MB + ROOT_SIZE_MB) * 2048)), size=$((USR_SIZE_MB * 2048)), type=83
${OUTPUT}p4 : start=$((2048 + (BOOT_SIZE_MB + ROOT_SIZE_MB + USR_SIZE_MB) * 2048)), size=$((HOME_SIZE_MB * 2048)), type=83
EOF
    else
        log_info "No partition tool found, creating raw image"
    fi
}

# Setup loop device and mount (if running as root)
setup_filesystem() {
    if [ "$(id -u)" != "0" ]; then
        log_info "Not running as root, skipping filesystem creation"
        log_info "Disk image created without filesystem"
        return
    fi

    log_info "Setting up loop device..."
    LOOP=$(losetup -f --show -P "$OUTPUT")

    if [ -n "$LOOP" ]; then
        log_info "Loop device: $LOOP"

        # Format partitions if possible
        if command -v mkfs.ext2 &> /dev/null; then
            log_info "Formatting partitions..."
            mkfs.ext2 -F "${LOOP}p1" 2>/dev/null || true
            mkfs.ext2 -F "${LOOP}p2" 2>/dev/null || true
            mkfs.ext2 -F "${LOOP}p3" 2>/dev/null || true
            mkfs.ext2 -F "${LOOP}p4" 2>/dev/null || true
        fi

        # Mount and copy files
        log_info "Mounting and copying files..."
        MOUNT_ROOT=$(mktemp -d)

        mount "${LOOP}p2" "$MOUNT_ROOT"
        mkdir -p "$MOUNT_ROOT"/{boot,dev,etc,bin,sbin,usr,var,tmp,proc,home,root,service}

        # Copy files from ISO root
        if [ -d "$DESTDIR/iso_root" ]; then
            cp -a "$DESTDIR/iso_root"/* "$MOUNT_ROOT/" 2>/dev/null || true
        fi

        # Unmount
        umount "$MOUNT_ROOT"
        rmdir "$MOUNT_ROOT"

        # Detach loop device
        losetup -d "$LOOP"
    fi
}

# Print summary
print_summary() {
    local size=$(du -h "$OUTPUT" 2>/dev/null | cut -f1)

    echo ""
    echo "========================================"
    echo "Disk Image Created"
    echo "========================================"
    echo "Output:     $OUTPUT"
    echo "Size:       $size"
    echo ""
    echo "Partition Layout:"
    echo "  p1: boot    ($BOOT_SIZE_MB MB)"
    echo "  p2: root    ($ROOT_SIZE_MB MB)"
    echo "  p3: /usr    ($USR_SIZE_MB MB)"
    echo "  p4: /home   ($HOME_SIZE_MB MB)"
    echo ""
    echo "To use with QEMU:"
    echo "  qemu-system-riscv64 -machine virt -m 256M \\"
    echo "    -bios default -nographic \\"
    echo "    -kernel /path/to/kernel \\"
    echo "    -drive file=$OUTPUT,format=raw,id=hd0 \\"
    echo "    -device virtio-blk-device,drive=hd0"
    echo "========================================"
}

# Main
main() {
    create_disk
    create_partitions
    setup_filesystem
    print_summary

    log_info "Done!"
}

main "$@"
