#!/bin/bash

# MINIX 3 RISC-V 64-bit Bootable Image Creation Script
# This script creates a minimal bootable MINIX RISC-V image for QEMU

set -e

echo "=== MINIX 3 RISC-V 64-bit Image Creation ==="

# Configuration
IMAGE_SIZE="256M"
IMAGE_FILE="minix-riscv64.img"
KERNEL_FILE="/home/donz/minix/minix/kernel/kernel"
ROOTFS_DIR="/tmp/minix-rootfs"

# Check if we have the tools
if ! command -v qemu-img &> /dev/null; then
    echo "ERROR: qemu-img not found. Please install QEMU tools."
    exit 1
fi

# Check if kernel exists
if [ ! -f "$KERNEL_FILE" ]; then
    echo "WARNING: Kernel not found at $KERNEL_FILE"
    echo "Will create placeholder for demonstration."
fi

echo "Creating image file..."
qemu-img create -f raw "$IMAGE_FILE" "$IMAGE_SIZE"

echo "Partitioning the image..."
# Create a simple partition table
cat > /tmp/partition.sfdisk << EOF
label: dos
label-id: 0x6f657463
unit: sectors

start=2048, type=83, bootable
EOF

sfdisk "$IMAGE_FILE" < /tmp/partition.sfdisk

# Set up loop device
LOOP_DEV=$(losetup --show -f -P "$IMAGE_FILE")
echo "Using loop device: $LOOP_DEV"

# Create filesystem
echo "Creating filesystem..."
mkfs.ext4 -L MINIX-RISCV "${LOOP_DEV}p1"

# Mount and populate
mkdir -p "$ROOTFS_DIR"
mount "${LOOP_DEV}p1" "$ROOTFS_DIR"

echo "Creating directory structure..."
mkdir -p "$ROOTFS_DIR"/{boot,bin,sbin,etc,dev,proc,sys,tmp,var,usr/{bin,sbin,lib}}

# Copy kernel if it exists
if [ -f "$KERNEL_FILE" ]; then
    echo "Copying kernel..."
    cp "$KERNEL_FILE" "$ROOTFS_DIR/boot/"
else
    echo "Creating placeholder kernel image..."
    dd if=/dev/zero of="$ROOTFS_DIR/boot/kernel" bs=1K count=32
fi

# Create minimal boot config
cat > "$ROOTFS_DIR/boot/boot.cfg" << EOF
# MINIX RISC-V 64-bit Boot Configuration
timeout=5
default=0

title=MINIX 3 RISC-V 64-bit
    kernel=/boot/kernel
    root=LABEL=MINIX-RISCV
    rootflags=rw
EOF

# Create minimal init system
cat > "$ROOTFS_DIR/init" << 'EOF'
#!/bin/sh

echo "=== MINIX 3 RISC-V 64-bit ==="
echo "Minimal init system running..."

# Mount essential filesystems
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t tmpfs tmpfs /tmp

# Create device nodes
echo "Creating device nodes..."
mknod /dev/console c 5 1
mknod /dev/null c 1 3
mknod /dev/zero c 1 5
mknod /dev/tty c 5 0

# Start shell
echo "Starting shell..."
exec /bin/sh
EOF
chmod +x "$ROOTFS_DIR/init"

# Create busybox binary (placeholder)
echo "Creating placeholder binaries..."
cat > "$ROOTFS_DIR/bin/sh" << 'EOF'
#!/bin/sh
echo "MINIX RISC-V 64-bit Shell"
echo "Type 'exit' to halt the system"
while true; do
    echo -n "minix-riscv# "
    read cmd
    case "$cmd" in
        "exit") echo "Halting system..."; exit 0 ;;
        "ps") echo "PID  COMMAND"; echo "1    init"; echo "2    sh" ;;
        "ls") echo "bin  boot  dev  etc  proc  sys  tmp  usr  var" ;;
        *) echo "Command: $cmd" ;;
    esac
done
EOF
chmod +x "$ROOTFS_DIR/bin/sh"

# Create /etc/fstab
cat > "$ROOTFS_DIR/etc/fstab" << EOF
# MINIX RISC-V fstab
LABEL=MINIX-RISCV  /         ext4    rw              0 1
proc              /proc     proc    defaults        0 0
sysfs             /sys      sysfs   defaults        0 0
tmpfs             /tmp      tmpfs   defaults        0 0
EOF

# Unmount
echo "Cleaning up..."
umount "$ROOTFS_DIR"
rmdir "$ROOTFS_DIR"
losetup -d "$LOOP_DEV

echo ""
echo "=== Image Creation Complete ==="
echo "Image file: $IMAGE_FILE"
echo ""
echo "To run the image with QEMU:"
echo "  qemu-system-riscv64 -machine virt -nographic -bios none -kernel $HOME/minix/obj/destdir.evbriscv64/boot/kernel -m 512M -drive file=$IMAGE_FILE,if=none,format=raw,id=hd0 -device virtio-blk-device,drive=hd0 -append 'root=/dev/vda rw console=ttyS0'"
echo ""
echo "Note: You need a compiled kernel first. Run:"
echo "  cd /home/donz/minix/minix/kernel && make MACHINE=evbriscv64 MACHINE_ARCH=riscv64"