#!/bin/sh
#
# MINIX 3 Installation Script for RISC-V 64-bit
#
# This script installs MINIX 3 to a disk from the installation media.
#

set -e

# Configuration
INSTALL_ROOT="/mnt"
ROOT_SIZE_MB=64
USR_SIZE_MB=128
HOME_SIZE_MB=32
SWAP_SIZE_MB=64

# Colors
if [ -t 1 ]; then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    YELLOW='\033[1;33m'
    NC='\033[0m'
else
    RED=''
    GREEN=''
    YELLOW=''
    NC=''
fi

echo_info() { echo "${GREEN}[INFO]${NC} $1"; }
echo_warn() { echo "${YELLOW}[WARN]${NC} $1"; }
echo_error() { echo "${RED}[ERROR]${NC} $1"; }

# Print banner
print_banner() {
    cat << 'EOF'

  MINIX 3 Installer
  RISC-V 64-bit Edition
  =====================

EOF
}

# Detect available disks
detect_disks() {
    echo_info "Detecting disks..."

    DISKS=""
    for disk in /dev/c[0-9]d[0-9]; do
        if [ -b "$disk" ]; then
            DISKS="$DISKS $disk"
            # Get disk size if possible
            size=$(fdisk -l "$disk" 2>/dev/null | head -1 | awk '{print $3, $4}' || echo "unknown size")
            echo "  $disk: $size"
        fi
    done

    if [ -z "$DISKS" ]; then
        # Try VirtIO naming
        for disk in /dev/vd[a-z]; do
            if [ -b "$disk" ]; then
                DISKS="$DISKS $disk"
                echo "  $disk"
            fi
        done
    fi

    if [ -z "$DISKS" ]; then
        echo_error "No disks found!"
        exit 1
    fi
}

# Select installation disk
select_disk() {
    echo ""
    echo "Available disks:$DISKS"
    echo ""
    echo "Enter disk to install to (e.g., /dev/c0d0): "
    read TARGET_DISK

    if [ ! -b "$TARGET_DISK" ]; then
        echo_error "Invalid disk: $TARGET_DISK"
        exit 1
    fi

    echo ""
    echo_warn "WARNING: All data on $TARGET_DISK will be destroyed!"
    echo "Continue? (yes/no): "
    read confirm

    if [ "$confirm" != "yes" ]; then
        echo "Installation cancelled."
        exit 0
    fi
}

# Partition disk
partition_disk() {
    echo_info "Partitioning disk $TARGET_DISK..."

    # Calculate partition sizes
    # Layout: boot(8MB) + root(64MB) + usr(128MB) + home(32MB) + swap(64MB)

    # Use fdisk or minix partition tool
    if command -v fdisk >/dev/null 2>&1; then
        # Create MBR partition table
        fdisk "$TARGET_DISK" << EOF
o
n
p
1

+${ROOT_SIZE_MB}M
n
p
2

+${USR_SIZE_MB}M
n
p
3

+${HOME_SIZE_MB}M
n
e


n

+${SWAP_SIZE_MB}M
t
5
82
w
EOF
    else
        echo_warn "fdisk not found, creating simple partition layout"
        # Simple partition with MINIX tools
        partition "$TARGET_DISK" 0 0 "$ROOT_SIZE_MB"M
    fi

    ROOT_PART="${TARGET_DISK}p0"
    USR_PART="${TARGET_DISK}p1"
    HOME_PART="${TARGET_DISK}p2"
    SWAP_PART="${TARGET_DISK}p5"

    echo_info "Partitions created"
}

# Format partitions
format_partitions() {
    echo_info "Formatting partitions..."

    # Format root partition
    echo_info "  Formatting $ROOT_PART (root)..."
    mkfs.mfs "$ROOT_PART"

    # Format usr partition
    echo_info "  Formatting $USR_PART (/usr)..."
    mkfs.mfs "$USR_PART"

    # Format home partition
    echo_info "  Formatting $HOME_PART (/home)..."
    mkfs.mfs "$HOME_PART"

    # Setup swap (if available)
    if [ -b "$SWAP_PART" ]; then
        echo_info "  Setting up swap on $SWAP_PART..."
        mkswap "$SWAP_PART" 2>/dev/null || true
    fi

    echo_info "Formatting complete"
}

# Mount partitions
mount_partitions() {
    echo_info "Mounting partitions..."

    mkdir -p "$INSTALL_ROOT"
    mount "$ROOT_PART" "$INSTALL_ROOT"

    mkdir -p "$INSTALL_ROOT/usr"
    mount "$USR_PART" "$INSTALL_ROOT/usr"

    mkdir -p "$INSTALL_ROOT/home"
    mount "$HOME_PART" "$INSTALL_ROOT/home"

    echo_info "Partitions mounted at $INSTALL_ROOT"
}

# Copy system files
copy_system() {
    echo_info "Copying system files..."

    # Create directory structure
    for dir in bin sbin lib etc dev proc tmp var root boot service; do
        mkdir -p "$INSTALL_ROOT/$dir"
    done

    mkdir -p "$INSTALL_ROOT/usr/bin"
    mkdir -p "$INSTALL_ROOT/usr/sbin"
    mkdir -p "$INSTALL_ROOT/usr/lib"
    mkdir -p "$INSTALL_ROOT/usr/share"
    mkdir -p "$INSTALL_ROOT/usr/include"
    mkdir -p "$INSTALL_ROOT/var/log"
    mkdir -p "$INSTALL_ROOT/var/run"
    mkdir -p "$INSTALL_ROOT/var/tmp"

    # Copy from installation media
    echo_info "  Copying kernel..."
    cp -p /boot/kernel "$INSTALL_ROOT/boot/"

    echo_info "  Copying servers..."
    cp -p /service/* "$INSTALL_ROOT/service/" 2>/dev/null || true

    echo_info "  Copying binaries..."
    cp -p /bin/* "$INSTALL_ROOT/bin/" 2>/dev/null || true
    cp -p /sbin/* "$INSTALL_ROOT/sbin/" 2>/dev/null || true
    cp -p /usr/bin/* "$INSTALL_ROOT/usr/bin/" 2>/dev/null || true
    cp -p /usr/sbin/* "$INSTALL_ROOT/usr/sbin/" 2>/dev/null || true

    echo_info "  Copying libraries..."
    cp -p /lib/* "$INSTALL_ROOT/lib/" 2>/dev/null || true
    cp -p /usr/lib/* "$INSTALL_ROOT/usr/lib/" 2>/dev/null || true

    echo_info "  Copying configuration..."
    cp -p /etc/* "$INSTALL_ROOT/etc/" 2>/dev/null || true
    cp -rp /etc/rc.d "$INSTALL_ROOT/etc/" 2>/dev/null || true

    # Set permissions
    chmod 1777 "$INSTALL_ROOT/tmp"
    chmod 1777 "$INSTALL_ROOT/var/tmp"
    chmod 700 "$INSTALL_ROOT/root"

    echo_info "System files copied"
}

# Configure installed system
configure_system() {
    echo_info "Configuring system..."

    # Create fstab
    cat > "$INSTALL_ROOT/etc/fstab" << EOF
# Device        Mountpoint      FStype  Options         Dump    Pass#
$ROOT_PART      /               mfs     rw              1       1
$USR_PART       /usr            mfs     rw              1       2
$HOME_PART      /home           mfs     rw              1       3
none            /tmp            mfs     rw,-s=64m       0       0
proc            /proc           procfs  rw              0       0
EOF

    # Add swap if available
    if [ -b "$SWAP_PART" ]; then
        echo "$SWAP_PART      none            swap    sw              0       0" >> "$INSTALL_ROOT/etc/fstab"
    fi

    # Create basic device nodes
    cd "$INSTALL_ROOT/dev"
    mknod console c 4 0 2>/dev/null || true
    mknod tty c 5 0 2>/dev/null || true
    mknod null c 1 3 2>/dev/null || true
    mknod zero c 1 5 2>/dev/null || true
    mknod random c 1 8 2>/dev/null || true
    mknod urandom c 1 9 2>/dev/null || true
    cd /

    # Set hostname
    echo "minix-riscv64" > "$INSTALL_ROOT/etc/hostname"

    echo_info "System configured"
}

# Install bootloader (for RISC-V, we rely on OpenSBI)
install_bootloader() {
    echo_info "Setting up boot configuration..."

    # Create boot configuration
    cat > "$INSTALL_ROOT/boot/boot.cfg" << EOF
# MINIX 3 RISC-V Boot Configuration
kernel=/boot/kernel
console=ttyS0,115200
root=$ROOT_PART
EOF

    echo_info "Boot configuration created"
    echo ""
    echo_warn "Note: RISC-V uses OpenSBI as firmware bootloader."
    echo_warn "To boot, use QEMU with: -bios default -kernel /boot/kernel"
}

# Unmount and finish
finish_install() {
    echo_info "Finishing installation..."

    sync

    umount "$INSTALL_ROOT/home" 2>/dev/null || true
    umount "$INSTALL_ROOT/usr" 2>/dev/null || true
    umount "$INSTALL_ROOT" 2>/dev/null || true

    echo ""
    echo "========================================"
    echo "  Installation Complete!"
    echo "========================================"
    echo ""
    echo "MINIX 3 has been installed to $TARGET_DISK"
    echo ""
    echo "To boot your new system with QEMU:"
    echo ""
    echo "  qemu-system-riscv64 -machine virt -m 256M \\"
    echo "    -bios default -nographic \\"
    echo "    -kernel /boot/kernel \\"
    echo "    -drive file=$TARGET_DISK,format=raw,id=hd0 \\"
    echo "    -device virtio-blk-device,drive=hd0 \\"
    echo "    -append \"root=$ROOT_PART console=ttyS0\""
    echo ""
    echo "Please remove the installation media and reboot."
    echo ""
}

# Main
main() {
    print_banner

    # Check root
    if [ "$(id -u)" != "0" ]; then
        echo_error "This script must be run as root"
        exit 1
    fi

    detect_disks
    select_disk
    partition_disk
    format_partitions
    mount_partitions
    copy_system
    configure_system
    install_bootloader
    finish_install
}

main "$@"
