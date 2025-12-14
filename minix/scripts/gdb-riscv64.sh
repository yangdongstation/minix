#!/bin/bash
#
# GDB launch script for MINIX/riscv64 debugging
#
# Usage:
#   ./gdb-riscv64.sh [kernel]
#

KERNEL="${1:-/usr/obj/minix/riscv64/minix/kernel/kernel}"
GDB="${GDB:-riscv64-unknown-elf-gdb}"

# Check if GDB exists
if ! command -v "$GDB" &> /dev/null; then
    # Try alternative names
    for alt in riscv64-linux-gnu-gdb riscv64-elf-gdb gdb-multiarch; do
        if command -v "$alt" &> /dev/null; then
            GDB="$alt"
            break
        fi
    done
fi

if ! command -v "$GDB" &> /dev/null; then
    echo "Error: RISC-V GDB not found"
    echo "Install with: apt install gdb-multiarch"
    exit 1
fi

# Create GDB init file
GDB_INIT=$(mktemp)
cat > "$GDB_INIT" << 'EOF'
# MINIX/riscv64 GDB initialization

# Connect to QEMU
target remote :1234

# Set architecture
set architecture riscv:rv64

# Useful breakpoints
# break kernel_main
# break exception_handler
# break panic

# Display registers on stop
define hook-stop
    info registers pc ra sp gp tp
end

# Helper to show trapframe
define show_trapframe
    print/x *(struct trapframe *)$a0
end

# Helper to show process
define show_proc
    print *(struct proc *)$a0
end

echo Connected to QEMU. Use 'continue' to start.\n
EOF

echo "Starting GDB with kernel: $KERNEL"
exec "$GDB" -x "$GDB_INIT" "$KERNEL"
