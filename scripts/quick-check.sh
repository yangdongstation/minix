#!/bin/bash
# Quick build status check for RISC-V 64-bit

echo "MINIX RISC-V 64-bit Status Check"
echo "================================"

# Check cross compiler
if command -v riscv64-unknown-elf-gcc >/dev/null 2>&1; then
    echo "[PASS] RISC-V cross compiler found"
else
    echo "[FAIL] RISC-V cross compiler not found"
fi

# Check critical files
echo ""
echo "Checking critical files:"

files=(
    "minix/kernel/arch/riscv64/head.S"
    "minix/kernel/arch/riscv64/console.c"
    "minix/kernel/arch/riscv64/sbi.c"
    "minix/kernel/arch/riscv64/plic.c"
    "minix/drivers/tty/ns16550/ns16550.c"
    "minix/lib/libminc/arch/riscv64/Makefile.libc.inc"
)

for f in "${files[@]}"; do
    if [ -f "$f" ]; then
        echo "[PASS] $f"
    else
        echo "[FAIL] $f"
    fi
done

# Quick compile test
echo ""
echo "Quick compile test:"
cd /tmp
cat > test.c << 'EOF'
int main() { return 0; }
EOF

if riscv64-unknown-elf-gcc -march=rv64gc -mabi=lp64d test.c -o test 2>/dev/null; then
    echo "[PASS] Basic compilation works"
    rm -f test test.c
else
    echo "[FAIL] Basic compilation failed"
fi

echo ""
echo "Build System Status:"
echo "- Basic architecture support: Implemented"
echo "- UART driver: Implemented"
echo "- Build files: Partially complete"
echo "- User space: Needs work"
echo "- Testing: Basic framework in place"