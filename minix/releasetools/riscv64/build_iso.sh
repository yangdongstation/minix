#!/bin/bash
#
# Build MINIX 3 ISO image for RISC-V 64-bit
#
# Usage:
#   ./build_iso.sh [options]
#
# Options:
#   -d DESTDIR     Destination directory (default: /usr/obj/minix/riscv64)
#   -o OUTPUT      Output ISO file (default: minix-riscv64.iso)
#   -k KERNEL      Kernel image path
#   -c             Clean build (remove existing files)
#   -v             Verbose output
#   -h             Show help
#

set -e

# Default values
DESTDIR="${DESTDIR:-/usr/obj/minix/riscv64}"
OUTPUT="${OUTPUT:-minix-riscv64.iso}"
KERNEL=""
CLEAN=0
VERBOSE=0

# Script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MINIX_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

usage() {
    cat << EOF
Usage: $0 [options]

Build MINIX 3 ISO image for RISC-V 64-bit.

Options:
    -d DESTDIR     Destination directory (default: /usr/obj/minix/riscv64)
    -o OUTPUT      Output ISO file (default: minix-riscv64.iso)
    -k KERNEL      Kernel image path (auto-detected if not specified)
    -c             Clean build (remove existing files first)
    -v             Verbose output
    -h             Show this help message

Examples:
    $0                          # Build with defaults
    $0 -o my-minix.iso          # Custom output filename
    $0 -c -v                    # Clean verbose build
EOF
    exit 0
}

# Parse arguments
while getopts "d:o:k:cvh" opt; do
    case $opt in
        d) DESTDIR="$OPTARG" ;;
        o) OUTPUT="$OPTARG" ;;
        k) KERNEL="$OPTARG" ;;
        c) CLEAN=1 ;;
        v) VERBOSE=1 ;;
        h) usage ;;
        *) usage ;;
    esac
done

# Check for required tools
check_tools() {
    local missing=0

    for tool in mkisofs genisoimage xorriso; do
        if command -v "$tool" &> /dev/null; then
            MKISO="$tool"
            break
        fi
    done

    if [ -z "$MKISO" ]; then
        log_error "No ISO creation tool found (mkisofs, genisoimage, or xorriso)"
        missing=1
    fi

    if ! command -v riscv64-unknown-elf-objcopy &> /dev/null; then
        # Try alternatives
        for alt in riscv64-linux-gnu-objcopy riscv64-elf-objcopy; do
            if command -v "$alt" &> /dev/null; then
                OBJCOPY="$alt"
                break
            fi
        done
        if [ -z "$OBJCOPY" ]; then
            log_warn "RISC-V objcopy not found, binary stripping disabled"
            OBJCOPY="true"  # no-op
        fi
    else
        OBJCOPY="riscv64-unknown-elf-objcopy"
    fi

    return $missing
}

# Clean previous build
clean_build() {
    if [ "$CLEAN" -eq 1 ]; then
        log_info "Cleaning previous build..."
        rm -rf "$DESTDIR/iso_root"
        rm -f "$OUTPUT"
    fi
}

# Create ISO directory structure
create_iso_structure() {
    log_info "Creating ISO directory structure..."

    local ISO_ROOT="$DESTDIR/iso_root"

    # Create directories
    mkdir -p "$ISO_ROOT"/{boot,dev,etc,bin,sbin,usr,var,tmp,proc,home,root}
    mkdir -p "$ISO_ROOT"/usr/{bin,sbin,lib,share,include}
    mkdir -p "$ISO_ROOT"/var/{log,run,tmp}
    mkdir -p "$ISO_ROOT"/etc/{rc.d,defaults}

    # Set permissions
    chmod 1777 "$ISO_ROOT/tmp"
    chmod 1777 "$ISO_ROOT/var/tmp"
    chmod 700 "$ISO_ROOT/root"
}

# Copy kernel
copy_kernel() {
    log_info "Copying kernel..."

    local ISO_ROOT="$DESTDIR/iso_root"

    # Find kernel if not specified
    if [ -z "$KERNEL" ]; then
        KERNEL="$DESTDIR/minix/kernel/kernel"
        if [ ! -f "$KERNEL" ]; then
            KERNEL="$MINIX_ROOT/../obj/minix/riscv64/minix/kernel/kernel"
        fi
    fi

    if [ -f "$KERNEL" ]; then
        cp "$KERNEL" "$ISO_ROOT/boot/kernel"
        log_info "Kernel copied from: $KERNEL"
    else
        log_warn "Kernel not found at $KERNEL"
        log_warn "Creating placeholder kernel..."
        touch "$ISO_ROOT/boot/kernel"
    fi
}

# Copy system files
copy_system_files() {
    log_info "Copying system files..."

    local ISO_ROOT="$DESTDIR/iso_root"

    # Copy RISC-V specific configuration files
    if [ -f "$MINIX_ROOT/minix/etc/rc.conf.riscv64" ]; then
        cp "$MINIX_ROOT/minix/etc/rc.conf.riscv64" "$ISO_ROOT/etc/rc.conf"
    fi

    if [ -f "$MINIX_ROOT/minix/etc/fstab.riscv64" ]; then
        cp "$MINIX_ROOT/minix/etc/fstab.riscv64" "$ISO_ROOT/etc/fstab"
    fi

    if [ -f "$MINIX_ROOT/minix/etc/ttys.riscv64" ]; then
        cp "$MINIX_ROOT/minix/etc/ttys.riscv64" "$ISO_ROOT/etc/ttys"
    fi

    if [ -f "$MINIX_ROOT/minix/etc/devmap.riscv64" ]; then
        cp "$MINIX_ROOT/minix/etc/devmap.riscv64" "$ISO_ROOT/etc/devmap"
    fi

    # Copy rc.d scripts
    if [ -d "$MINIX_ROOT/minix/etc/rc.d" ]; then
        cp -r "$MINIX_ROOT/minix/etc/rc.d"/* "$ISO_ROOT/etc/rc.d/" 2>/dev/null || true
    fi

    # Create essential files
    cat > "$ISO_ROOT/etc/passwd" << 'EOF'
root:x:0:0:Charlie &:/root:/bin/sh
daemon:x:1:1:The devil himself:/:/sbin/nologin
operator:x:2:5:System &:/:/sbin/nologin
bin:x:3:7:Binaries Commands and Source:/:/sbin/nologin
nobody:x:32767:32766:Unprivileged user:/nonexistent:/sbin/nologin
EOF

    cat > "$ISO_ROOT/etc/group" << 'EOF'
wheel:*:0:root
daemon:*:1:daemon
kmem:*:2:
sys:*:3:
tty:*:4:
operator:*:5:root
bin:*:7:
nogroup:*:32766:
nobody:*:32767:
EOF

    cat > "$ISO_ROOT/etc/hosts" << 'EOF'
127.0.0.1       localhost
::1             localhost
EOF

    # Create motd
    cat > "$ISO_ROOT/etc/motd" << 'EOF'

Welcome to MINIX 3 (RISC-V 64-bit)

This is an experimental port to the RISC-V architecture.
Target platform: QEMU virt machine

For more information: https://www.minix3.org/

EOF

    # Create profile
    cat > "$ISO_ROOT/etc/profile" << 'EOF'
PATH=/bin:/sbin:/usr/bin:/usr/sbin
export PATH
PS1='# '
export PS1
TERM=vt100
export TERM
EOF

    # Create shells
    echo "/bin/sh" > "$ISO_ROOT/etc/shells"
}

# Copy servers
copy_servers() {
    log_info "Copying servers..."

    local ISO_ROOT="$DESTDIR/iso_root"
    local SERVERS_DIR="$DESTDIR/minix/servers"

    mkdir -p "$ISO_ROOT/service"

    # List of essential servers
    local SERVERS="pm vfs vm rs ds is sched"

    for server in $SERVERS; do
        if [ -f "$SERVERS_DIR/$server/$server" ]; then
            cp "$SERVERS_DIR/$server/$server" "$ISO_ROOT/service/"
            log_info "  Copied server: $server"
        fi
    done
}

# Copy drivers
copy_drivers() {
    log_info "Copying drivers..."

    local ISO_ROOT="$DESTDIR/iso_root"
    local DRIVERS_DIR="$DESTDIR/minix/drivers"

    mkdir -p "$ISO_ROOT/service"

    # VirtIO block driver
    if [ -f "$DRIVERS_DIR/storage/virtio_blk_mmio/virtio_blk_mmio" ]; then
        cp "$DRIVERS_DIR/storage/virtio_blk_mmio/virtio_blk_mmio" "$ISO_ROOT/service/"
        log_info "  Copied driver: virtio_blk_mmio"
    fi

    # NS16550 UART driver
    if [ -f "$DRIVERS_DIR/tty/ns16550/ns16550" ]; then
        cp "$DRIVERS_DIR/tty/ns16550/ns16550" "$ISO_ROOT/service/"
        log_info "  Copied driver: ns16550"
    fi

    # Memory driver
    if [ -f "$DRIVERS_DIR/system/memory/memory" ]; then
        cp "$DRIVERS_DIR/system/memory/memory" "$ISO_ROOT/service/"
        log_info "  Copied driver: memory"
    fi

    # Log driver
    if [ -f "$DRIVERS_DIR/system/log/log" ]; then
        cp "$DRIVERS_DIR/system/log/log" "$ISO_ROOT/service/"
        log_info "  Copied driver: log"
    fi
}

# Copy utilities
copy_utilities() {
    log_info "Copying utilities..."

    local ISO_ROOT="$DESTDIR/iso_root"
    local BIN_DIR="$DESTDIR/bin"
    local SBIN_DIR="$DESTDIR/sbin"
    local USR_BIN="$DESTDIR/usr/bin"
    local USR_SBIN="$DESTDIR/usr/sbin"

    # Essential binaries
    local BINARIES="sh ls cat cp mv rm mkdir rmdir echo test pwd date sleep kill"
    local SBINARIES="init mount umount reboot halt shutdown"

    for bin in $BINARIES; do
        if [ -f "$BIN_DIR/$bin" ]; then
            cp "$BIN_DIR/$bin" "$ISO_ROOT/bin/"
        fi
    done

    for sbin in $SBINARIES; do
        if [ -f "$SBIN_DIR/$sbin" ]; then
            cp "$SBIN_DIR/$sbin" "$ISO_ROOT/sbin/"
        fi
    done

    # Create symlinks for common commands
    (cd "$ISO_ROOT/bin" && ln -sf test "[" 2>/dev/null || true)
}

# Create boot configuration
create_boot_config() {
    log_info "Creating boot configuration..."

    local ISO_ROOT="$DESTDIR/iso_root"

    # OpenSBI expects kernel in specific format
    # Create a simple boot script for QEMU
    cat > "$ISO_ROOT/boot/boot.cfg" << 'EOF'
# MINIX 3 RISC-V 64-bit Boot Configuration
# For use with QEMU virt platform

kernel=/boot/kernel
console=ttyS0,115200
root=/dev/c0d0p0

# Boot parameters
kernel_args=console=ttyS0 root=/dev/vda rw
EOF

    # Create a simple init script
    cat > "$ISO_ROOT/etc/rc" << 'EOF'
#!/bin/sh
#
# MINIX 3 RISC-V boot script
#

echo "MINIX 3 RISC-V 64-bit starting..."

# Mount filesystems
/sbin/mount -a

# Start system services
if [ -x /etc/rc.d/riscv64_init ]; then
    /etc/rc.d/riscv64_init start
fi

# Start login
exec /sbin/init
EOF
    chmod +x "$ISO_ROOT/etc/rc"
}

# Create ramdisk image
create_ramdisk() {
    log_info "Creating ramdisk image..."

    local ISO_ROOT="$DESTDIR/iso_root"
    local RAMDISK="$DESTDIR/ramdisk.img"
    local RAMDISK_SIZE=32  # MB

    # Create empty ramdisk
    dd if=/dev/zero of="$RAMDISK" bs=1M count=$RAMDISK_SIZE 2>/dev/null

    log_info "Ramdisk created: $RAMDISK ($RAMDISK_SIZE MB)"
}

# Create the ISO image
create_iso() {
    log_info "Creating ISO image..."

    local ISO_ROOT="$DESTDIR/iso_root"

    # Use appropriate tool
    case "$MKISO" in
        xorriso)
            xorriso -as mkisofs \
                -o "$OUTPUT" \
                -R -J \
                -V "MINIX3_RISCV64" \
                -boot-info-table \
                "$ISO_ROOT"
            ;;
        genisoimage|mkisofs)
            $MKISO \
                -o "$OUTPUT" \
                -R -J \
                -V "MINIX3_RISCV64" \
                -boot-info-table \
                "$ISO_ROOT"
            ;;
    esac

    if [ -f "$OUTPUT" ]; then
        local size=$(du -h "$OUTPUT" | cut -f1)
        log_info "ISO created: $OUTPUT ($size)"
    else
        log_error "Failed to create ISO"
        exit 1
    fi
}

# Create disk image for QEMU
create_disk_image() {
    log_info "Creating disk image for QEMU..."

    local DISK_IMG="$DESTDIR/minix-riscv64.img"
    local DISK_SIZE=256  # MB

    # Create raw disk image
    dd if=/dev/zero of="$DISK_IMG" bs=1M count=$DISK_SIZE 2>/dev/null

    log_info "Disk image created: $DISK_IMG ($DISK_SIZE MB)"

    # Note: Actual partitioning and filesystem creation would be done
    # during installation or with separate tools
}

# Print summary
print_summary() {
    echo ""
    echo "========================================="
    echo "Build Summary"
    echo "========================================="
    echo "ISO Image:    $OUTPUT"
    echo "Disk Image:   $DESTDIR/minix-riscv64.img"
    echo "ISO Root:     $DESTDIR/iso_root"
    echo ""
    echo "To test with QEMU:"
    echo "  qemu-system-riscv64 -machine virt -m 256M \\"
    echo "    -bios default -nographic \\"
    echo "    -kernel $DESTDIR/iso_root/boot/kernel \\"
    echo "    -drive file=$DESTDIR/minix-riscv64.img,format=raw,id=hd0 \\"
    echo "    -device virtio-blk-device,drive=hd0"
    echo "========================================="
}

# Main
main() {
    log_info "Building MINIX 3 ISO for RISC-V 64-bit"
    log_info "MINIX Root: $MINIX_ROOT"
    log_info "Destination: $DESTDIR"

    check_tools || exit 1

    clean_build
    create_iso_structure
    copy_kernel
    copy_system_files
    copy_servers
    copy_drivers
    copy_utilities
    create_boot_config
    create_ramdisk
    create_disk_image
    create_iso

    print_summary

    log_info "Build completed successfully!"
}

main "$@"
