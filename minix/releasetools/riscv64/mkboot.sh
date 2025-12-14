#!/bin/bash
#
# Create bootable kernel image for MINIX 3 RISC-V 64-bit
#
# This script prepares the kernel binary for booting with OpenSBI/QEMU.
#
# Usage:
#   ./mkboot.sh [options]
#
# Options:
#   -k KERNEL      Input kernel ELF file
#   -o OUTPUT      Output boot image
#   -f FORMAT      Output format (elf, binary, uimage)
#   -h             Show help
#

set -e

# Default values
KERNEL=""
OUTPUT=""
FORMAT="elf"

# Script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MINIX_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Cross tools
CROSS_PREFIX="${CROSS_PREFIX:-riscv64-unknown-elf-}"
OBJCOPY="${CROSS_PREFIX}objcopy"
OBJDUMP="${CROSS_PREFIX}objdump"

log_info() {
    echo "[INFO] $1"
}

log_error() {
    echo "[ERROR] $1" >&2
}

usage() {
    cat << EOF
Usage: $0 [options]

Create bootable kernel image for MINIX 3 RISC-V 64-bit.

Options:
    -k KERNEL      Input kernel ELF file
    -o OUTPUT      Output boot image
    -f FORMAT      Output format: elf (default), binary, uimage
    -h             Show help

Formats:
    elf     - Keep as ELF (for QEMU -kernel)
    binary  - Raw binary image
    uimage  - U-Boot image (requires mkimage)

Examples:
    $0 -k kernel -o kernel.bin -f binary
    $0 -k kernel -o kernel.ub -f uimage
EOF
    exit 0
}

# Parse arguments
while getopts "k:o:f:h" opt; do
    case $opt in
        k) KERNEL="$OPTARG" ;;
        o) OUTPUT="$OPTARG" ;;
        f) FORMAT="$OPTARG" ;;
        h) usage ;;
        *) usage ;;
    esac
done

# Find objcopy
find_tools() {
    if ! command -v "$OBJCOPY" &> /dev/null; then
        for alt in riscv64-linux-gnu-objcopy riscv64-elf-objcopy; do
            if command -v "$alt" &> /dev/null; then
                OBJCOPY="$alt"
                OBJDUMP="${alt%objcopy}objdump"
                break
            fi
        done
    fi

    if ! command -v "$OBJCOPY" &> /dev/null; then
        log_error "RISC-V objcopy not found"
        exit 1
    fi
}

# Find kernel
find_kernel() {
    if [ -z "$KERNEL" ]; then
        # Try common locations
        for path in \
            "$MINIX_ROOT/../obj/minix/riscv64/minix/kernel/kernel" \
            "/usr/obj/minix/riscv64/minix/kernel/kernel" \
            "$MINIX_ROOT/minix/kernel/kernel"
        do
            if [ -f "$path" ]; then
                KERNEL="$path"
                break
            fi
        done
    fi

    if [ -z "$KERNEL" ] || [ ! -f "$KERNEL" ]; then
        log_error "Kernel not found. Specify with -k option."
        exit 1
    fi

    log_info "Using kernel: $KERNEL"
}

# Set default output name
set_output() {
    if [ -z "$OUTPUT" ]; then
        case "$FORMAT" in
            binary)  OUTPUT="kernel.bin" ;;
            uimage)  OUTPUT="kernel.ub" ;;
            *)       OUTPUT="kernel" ;;
        esac
    fi
}

# Display kernel info
show_kernel_info() {
    log_info "Kernel information:"

    if command -v "$OBJDUMP" &> /dev/null; then
        # Get entry point
        entry=$($OBJDUMP -f "$KERNEL" 2>/dev/null | grep "start address" | awk '{print $NF}')
        echo "  Entry point: $entry"

        # Get sections
        echo "  Sections:"
        $OBJDUMP -h "$KERNEL" 2>/dev/null | grep -E "^\s+[0-9]+" | \
            awk '{printf "    %-12s 0x%s (%s bytes)\n", $2, $4, $3}'
    fi

    # File size
    size=$(stat -f%z "$KERNEL" 2>/dev/null || stat -c%s "$KERNEL" 2>/dev/null)
    echo "  File size: $size bytes"
}

# Create ELF output (just copy)
create_elf() {
    log_info "Creating ELF image..."
    cp "$KERNEL" "$OUTPUT"
    log_info "ELF image created: $OUTPUT"
}

# Create binary output
create_binary() {
    log_info "Creating binary image..."
    $OBJCOPY -O binary "$KERNEL" "$OUTPUT"
    log_info "Binary image created: $OUTPUT"
}

# Create U-Boot image
create_uimage() {
    log_info "Creating U-Boot image..."

    # Check for mkimage
    if ! command -v mkimage &> /dev/null; then
        log_error "mkimage not found. Install u-boot-tools."
        exit 1
    fi

    # First create binary
    local bin_tmp=$(mktemp)
    $OBJCOPY -O binary "$KERNEL" "$bin_tmp"

    # Get entry point
    local entry=$($OBJDUMP -f "$KERNEL" | grep "start address" | awk '{print $NF}')

    # Create uImage
    mkimage -A riscv -O linux -T kernel -C none \
            -a 0x80200000 -e "${entry:-0x80200000}" \
            -n "MINIX3-RISCV64" \
            -d "$bin_tmp" "$OUTPUT"

    rm -f "$bin_tmp"
    log_info "U-Boot image created: $OUTPUT"
}

# Create combined image with OpenSBI
create_opensbi_payload() {
    log_info "Creating OpenSBI payload..."

    # This would require OpenSBI source to build a custom firmware
    # For now, just document the process
    cat << 'EOF'
Note: To create an OpenSBI payload with MINIX kernel:

1. Download OpenSBI source:
   git clone https://github.com/riscv-software-src/opensbi.git

2. Build with MINIX as payload:
   make CROSS_COMPILE=riscv64-unknown-elf- \
        PLATFORM=generic \
        FW_PAYLOAD_PATH=/path/to/minix/kernel

3. The output fw_payload.elf can be used directly with QEMU:
   qemu-system-riscv64 -machine virt -m 256M -nographic \
        -bios opensbi/build/platform/generic/firmware/fw_payload.elf
EOF
}

# Print summary
print_summary() {
    echo ""
    echo "========================================"
    echo "Boot Image Summary"
    echo "========================================"
    echo "Input:      $KERNEL"
    echo "Output:     $OUTPUT"
    echo "Format:     $FORMAT"

    if [ -f "$OUTPUT" ]; then
        local size=$(stat -f%z "$OUTPUT" 2>/dev/null || stat -c%s "$OUTPUT" 2>/dev/null)
        echo "Size:       $size bytes"
    fi

    echo ""
    echo "To boot with QEMU:"
    echo "  qemu-system-riscv64 -machine virt -m 256M \\"
    echo "    -bios default -nographic \\"
    echo "    -kernel $OUTPUT"
    echo "========================================"
}

# Main
main() {
    find_tools
    find_kernel
    set_output
    show_kernel_info

    case "$FORMAT" in
        elf)
            create_elf
            ;;
        binary|bin)
            create_binary
            ;;
        uimage|uboot)
            create_uimage
            ;;
        opensbi)
            create_opensbi_payload
            ;;
        *)
            log_error "Unknown format: $FORMAT"
            usage
            ;;
    esac

    print_summary
}

main "$@"
