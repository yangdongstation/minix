#!/bin/bash

# MINIX 3 RISC-V 64-bit QEMU Launcher Script

set -e

echo "=== MINIX 3 RISC-V 64-bit QEMU Launcher ==="

# Check for QEMU
if ! command -v qemu-system-riscv64 &> /dev/null; then
    echo "ERROR: qemu-system-riscv64 not found. Please install QEMU with RISC-V support."
    echo ""
    echo "On Ubuntu/Debian:"
    echo "  sudo apt-get install qemu-system-riscv64"
    echo ""
    echo "On Fedora/RHEL/CentOS:"
    echo "  sudo dnf install qemu-system-riscv"
    exit 1
fi

# Check if OpenSBI is available
OPENSBI_BIOS=""
for path in \
    /usr/share/qemu/opensbi-riscv64-generic-fw_dynamic.bin \
    /usr/share/opensbi/generic/fw_dynamic.bin \
    /usr/local/share/opensbi/generic/fw_dynamic.bin; do
    if [ -f "$path" ]; then
        OPENSBI_BIOS="$path"
        break
    fi
done

# Kernel path
KERNEL_PATH="/home/donz/minix/minix/kernel/kernel"
KERNEL_PARAMS="console=ttyS0 earlycon=sbi"

# Create a simple root filesystem image
ROOTFS_IMAGE="/tmp/minix-riscv-rootfs.img"

echo "Setting up minimal root filesystem..."

# Create a simple ext4 filesystem
dd if=/dev/zero of="$ROOTFS_IMAGE" bs=1M count=64 2>/dev/null
mkfs.ext4 -F "$ROOTFS_IMAGE"

# Mount and populate
mkdir -p /tmp/minix-rootfs
mount -o loop "$ROOTFS_IMAGE" /tmp/minix-rootfs

# Create minimal directory structure
mkdir -p /tmp/minix-rootfs/{bin,sbin,etc,dev,proc,sys,tmp,usr,lib}

# Create minimal init
cat > /tmp/minix-rootfs/init << 'EOF'
#!/bin/sh
echo ""
echo "========================================"
echo "MINIX 3 RISC-V 64-bit - Minimal System"
echo "========================================"
echo ""
echo "This is a demonstration setup for MINIX 3 on RISC-V."
echo "The full MINIX system requires compilation of the kernel"
echo "and userland utilities."
echo ""
echo "Available commands:"
echo "  help     - Show this message"
echo "  ps       - Show processes"
echo "  ls       - List files"
echo "  exit     - Shutdown"
echo ""
while true; do
    echo -n "minix-riscv# "
    read cmd || break
    case "$cmd" in
        "help"|"h"|"?")
            echo "MINIX 3 RISC-V 64-bit Demo System"
            echo ""
            echo "To build and run the full MINIX system:"
            echo "  1. Build the toolchain:"
            echo "     ./build.sh -m evbriscv64 tools"
            echo ""
            echo "  2. Build the system:"
            echo "     MKPCI=no ./build.sh -U -m evbriscv64 distribution"
            echo ""
            echo "  3. Run with QEMU:"
            echo "     ./minix/scripts/qemu-riscv64.sh"
            ;;
        "ps")
            echo "PID  TTY          TIME CMD"
            echo "  1  ?        00:00:00 init"
            echo "  2  ?        00:00:00 sh"
            ;;
        "ls")
            echo "bin  dev  etc  init  proc  sys  tmp  usr"
            ;;
        "exit"|"quit")
            echo "Shutting down..."
            exit 0
            ;;
        "")
            continue
            ;;
        *)
            echo "Unknown command: $cmd"
            echo "Type 'help' for available commands"
            ;;
    esac
done
EOF
chmod +x /tmp/minix-rootfs/init

# Create device nodes
mknod /tmp/minix-rootfs/dev/console c 5 1
mknod /tmp/minix-rootfs/dev/null c 1 3

# Unmount
umount /tmp/minix-rootfs
rmdir /tmp/minix-rootfs

echo "Launching QEMU..."
echo ""

# QEMU command
QEMU_CMD="qemu-system-riscv64 \
    -machine virt \
    -nographic \
    -cpu rv64 \
    -m 512M \
    -bios $OPENSBI_BIOS \
    -kernel $KERNEL_PATH \
    -append '$KERNEL_PARAMS root=/dev/vda rw' \
    -drive file=$ROOTFS_IMAGE,if=virtio,format=raw \
    -netdev user,id=net0 \
    -device virtio-net-device,netdev=net0"

# Check if kernel exists
if [ ! -f "$KERNEL_PATH" ]; then
    echo "WARNING: Kernel not found at $KERNEL_PATH"
    echo ""
    echo "You need to build the MINIX kernel first:"
    echo ""
    echo "  cd /home/donz/minix/minix/kernel"
    echo "  make MACHINE=evbriscv64 MACHINE_ARCH=riscv64"
    echo ""
    echo "Or use the MINIX build script:"
    echo ""
    echo "  ./build.sh -U -m evbriscv64 distribution"
    echo ""
    echo "For now, launching QEMU without kernel..."
    QEMU_CMD="qemu-system-riscv64 \
        -machine virt \
        -nographic \
        -cpu rv64 \
        -m 512M \
        -bios $OPENSBI_BIOS"
fi

echo "QEMU command:"
echo "$QEMU_CMD"
echo ""

# Execute QEMU
eval "$QEMU_CMD"