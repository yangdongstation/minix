# MINIX Development and Debugging Scripts Directory (/minix/scripts)

## Overview

The `/minix/minix/scripts/` directory contains essential development, debugging, and testing scripts specifically designed for MINIX development. These scripts provide streamlined interfaces for common development tasks such as running MINIX in QEMU, debugging with GDB, automated testing, and system analysis specifically tailored for MINIX's microkernel architecture and RISC-V 64-bit platform support.

## Directory Structure

```
minix/scripts/
├── qemu-riscv64.sh    # QEMU launch script for RISC-V 64-bit
├── gdb-riscv64.sh     # GDB debugging script for RISC-V 64-bit
└── [additional development scripts]
```

## Key Development Scripts

### qemu-riscv64.sh - QEMU Launch Script
Comprehensive QEMU launcher for MINIX/RISC-V development with extensive configuration options.

**Key Features:**
- Multi-mode operation (normal, debug, single CPU)
- Configurable memory and CPU settings
- Network and graphics support
- Automated kernel and disk image detection
- Integration with MINIX build system
- GDB debugging support

**Script Architecture:**
```bash
#!/bin/bash
#
# QEMU launch script for MINIX/riscv64
#
# Usage: ./qemu-riscv64.sh [options]
#
# Options:
#   -d          Debug mode (wait for GDB)
#   -s          Single CPU mode
#   -m SIZE     Memory size (default: 256M)
#   -k KERNEL   Kernel image path
#   -i IMAGE    Disk image path
#   -n          Enable networking
#   -g          Enable graphics (virtio-gpu)

# Default configuration
MEMORY="256M"
CPUS=4
declare -a QEMU_ARGS=(
    -machine virt
    -cpu rv64
    -smp "$CPUS"
    -m "$MEMORY"
    -bios none
    -kernel "$KERNEL"
    -append "console=ttyS0"
    -serial stdio
)

# Platform-specific devices for RISC-V
add_riscv_devices() {
    # Add CLINT (Core Local Interruptor)
    QEMU_ARGS+=("-device loader,file=$OPENSBI,addr=0x80000000")
    
    # Add PLIC (Platform-Level Interrupt Controller)
    QEMU_ARGS+=("-device virtio-net-device,netdev=net0")
    
    # Add UART (NS16550 compatible)
    QEMU_ARGS+=("-serial mon:stdio")
    
    # Add VirtIO devices
    if [ "$NETWORK" -eq 1 ]; then
        QEMU_ARGS+=("-netdev user,id=net0,hostfwd=tcp::2222-:22")
    fi
}
```

**Usage Examples:**
```bash
# Basic launch
./qemu-riscv64.sh

# Debug mode with GDB support
./qemu-riscv64.sh -d

# Single CPU with custom memory
./qemu-riscv64.sh -s -m 512M

# With networking and custom kernel
./qemu-riscv64.sh -n -k /path/to/kernel -i /path/to/disk.img

# Graphics mode with virtio-gpu
./qemu-riscv64.sh -g -m 1G
```

### gdb-riscv64.sh - GDB Debugging Script
Advanced GDB debugging interface for MINIX/RISC-V kernel and user-space debugging.

**Key Features:**
- Automated GDB configuration for RISC-V
- Kernel and user-space debugging support
- Hardware breakpoint management
- Memory examination utilities
- Register state analysis
- Integration with QEMU debugging

**Script Implementation:**
```bash
#!/bin/bash
#
# GDB debugging script for MINIX/riscv64
#
# Usage: ./gdb-riscv64.sh [kernel-image]

GDB_SCRIPT=$(mktemp /tmp/gdb-minix-riscv64.XXXXXX)
KERNEL=${1:-"obj.evbriscv64/destdir.evbriscv64/boot/minix-riscv64/kernel"}

# Create GDB configuration
cat > "$GDB_SCRIPT" << 'EOF'
# RISC-V 64-bit specific configuration
set architecture riscv:rv64
set riscv use-compressed-breakpoints yes

# MINIX kernel debugging helpers
define minix_show_processes
    printf "MINIX Process Table:\n"
    set $i = 0
    while $i < NR_PROCS
        printf "PID %d: %s state=%d\n", \
               mproc[$i].mp_pid, \
               mproc[$i].mp_name, \
               mproc[$i].mp_flags
        set $i = $i + 1
    end
end

define minix_show_ipc
    printf "Recent IPC Messages:\n"
    # Implementation for showing IPC history
end

define minix_show_memory
    printf "Memory Layout:\n"
    printf "Kernel: %p-%p\n", kernel_base, kernel_top
    printf "User: %p-%p\n", user_base, user_top
end

# RISC-V specific helpers
define riscv_show_registers
    printf "RISC-V Registers:\n"
    info registers
    printf "Special registers:\n"
    printf "  pc: 0x%016llx\n", $pc
    printf "  sp: 0x%016llx\n", $sp
    printf "  ra: 0x%016llx\n", $ra
end

# Breakpoint definitions
break _main
break exception_handler
break ipc_sendrec

# Initialize debugging
target remote :1234
printf "MINIX RISC-V debugging ready\n"
printf "Use 'minix_show_processes' to show process table\n"
printf "Use 'riscv_show_registers' for RISC-V registers\n"
EOF

# Launch GDB
riscv64-unknown-elf-gdb -x "$GDB_SCRIPT" "$KERNEL"
rm -f "$GDB_SCRIPT"
```

**Advanced Debugging Features:**
```bash
# Connect to running QEMU instance
./gdb-riscv64.sh /path/to/kernel

# In GDB:
(gdb) minix_show_processes     # Show MINIX process table
(gdb) minix_show_ipc          # Show recent IPC messages
(gdb) riscv_show_registers    # Show RISC-V register state
(gdb) info breakpoints        # Show active breakpoints
(gdb) x/10x $sp               # Examine stack memory
```

## RISC-V 64-bit Specific Features

### QEMU Configuration for RISC-V
Specialized QEMU settings optimized for RISC-V 64-bit MINIX development:

```bash
# RISC-V specific QEMU arguments
configure_riscv_qemu() {
    # RISC-V 64-bit CPU with extensions
    QEMU_ARGS+=("-cpu rv64,msa=true")
    
    # Sv39 virtual memory (3-level page tables)
    QEMU_ARGS+=("-machine virt")
    
    # RISC-V specific devices
    QEMU_ARGS+=("-device loader,addr=0x80200000")
    
    # CLINT (Core Local Interruptor)
    QEMU_ARGS+=("-machine acpi=off")
    
    # PLIC configuration
    QEMU_ARGS+=("-machine mmio=on")
    
    # Memory layout for RISC-V
    QEMU_ARGS+=("-m $MEMORY")
    
    # Serial console (NS16550 compatible)
    QEMU_ARGS+=("-serial mon:stdio")
    
    # OpenSBI firmware
    if [ -n "$OPENSBI" ]; then
        QEMU_ARGS+=("-bios $OPENSBI")
    fi
}
```

### RISC-V Debugging Capabilities
Advanced debugging features specific to RISC-V architecture:

```bash
# RISC-V register examination
dump_riscv_registers() {
    local regs=("zero" "ra" "sp" "gp" "tp" "t0" "t1" "t2")
    local i
    
    echo "RISC-V General Purpose Registers:"
    for i in {0..31}; do
        local reg_name="x$i"
        if [ $i -lt ${#regs[@]} ]; then
            reg_name="${regs[$i]}($i)"
        fi
        echo "  $reg_name: $(riscv64-unknown-elf-gdb -batch -ex "p/x \$x$i" -ex "quit")"
    done
}

# RISC-V CSR (Control and Status Register) access
read_riscv_csr() {
    local csr_name=$1
    local csr_address=$2
    
    echo "Reading RISC-V CSR: $csr_name (0x$csr_address)"
    riscv64-unknown-elf-gdb -batch \
        -ex "set \$csr_val = {long}0x$csr_address" \
        -ex "p/x \$csr_val" \
        -ex "quit"
}

# Memory protection examination
check_riscv_memory_protection() {
    echo "RISC-V Memory Protection Status:"
    echo "Satp register: $(read_riscv_csr satp 0x180)"
    echo "Mstatus register: $(read_riscv_csr mstatus 0x300)"
    echo "Mie register: $(read_riscv_csr mie 0x304)"
}
```

## Advanced Development Features

### Automated Testing Integration
Scripts for automated testing of MINIX functionality:

```bash
# Automated kernel testing
run_kernel_tests() {
    local test_suite=${1:-"basic"}
    local timeout=${2:-300}
    
    echo "Starting MINIX kernel tests: $test_suite"
    
    # Start QEMU with test kernel
    ./qemu-riscv64.sh -k obj.evbriscv64/destdir.evbriscv64/boot/minix-riscv64/kernel-test \
                     -m 256M -s > test_output.log 2>&1 &
    
    local qemu_pid=$!
    
    # Wait for tests to complete
    local elapsed=0
    while [ $elapsed -lt $timeout ]; do
        if grep -q "TEST COMPLETE" test_output.log; then
            echo "Tests completed successfully"
            kill $qemu_pid 2>/dev/null
            return 0
        fi
        
        if grep -q "TEST FAILED" test_output.log; then
            echo "Tests failed - check test_output.log"
            kill $qemu_pid 2>/dev/null
            return 1
        fi
        
        sleep 1
        elapsed=$((elapsed + 1))
    done
    
    echo "Tests timed out after $timeout seconds"
    kill $qemu_pid 2>/dev/null
    return 1
}
```

### Performance Analysis Tools
Scripts for analyzing MINIX performance characteristics:

```bash
# Performance monitoring and analysis
analyze_performance() {
    local duration=${1:-60}
    local output_file="performance_${duration}s.log"
    
    echo "Starting $duration second performance analysis"
    
    # Start QEMU with performance monitoring
    ./qemu-riscv64.sh -m 512M -d > qemu_debug.log 2>&1 &
    local qemu_pid=$!
    
    # Connect GDB for performance analysis
    {
        echo "set pagination off"
        echo "set logging file $output_file"
        echo "set logging on"
        echo "target remote :1234"
        echo "printf \"Performance analysis started\\n\""
        
        # Sample performance data
        for i in $(seq 1 $duration); do
            echo "printf \"Sample $i at %d seconds\\n\", $i"
            echo "info registers"
            echo "backtrace"
            echo "minix_show_processes"
            echo "continue"
            echo "shell sleep 1"
        done
        
        echo "printf \"Performance analysis complete\\n\""
        echo "quit"
    } | riscv64-unknown-elf-gdb "$KERNEL"
    
    kill $qemu_pid 2>/dev/null
    
    echo "Performance data saved to $output_file"
}
```

### Memory Analysis Tools
Scripts for analyzing memory usage and allocation patterns:

```bash
# Memory usage analysis
analyze_memory_usage() {
    local pid=${1:-""}
    
    if [ -z "$pid" ]; then
        echo "Usage: $0 <process_id>"
        return 1
    fi
    
    echo "Analyzing memory usage for process $pid"
    
    # Connect to process via GDB
    riscv64-unknown-elf-gdb -batch -ex "attach $pid" \
        -ex "printf \"Process memory map:\\n\"" \
        -ex "info proc mappings" \
        -ex "printf \"Stack usage:\\n\"" \
        -ex "print (void*)\$sp" \
        -ex "print (void*)\$gp" \
        -ex "printf \"Heap analysis:\\n\"" \
        -ex "call malloc_stats()" \
        -ex "detach" \
        -ex "quit"
}

# Kernel memory analysis
analyze_kernel_memory() {
    echo "Analyzing kernel memory layout"
    
    ./gdb-riscv64.sh << 'EOF'
printf "Kernel Memory Analysis:\n"
printf "Text segment: %p-%p\n", &_text, &_etext
printf "Data segment: %p-%p\n", &_data, &_edata
printf "BSS segment: %p-%p\n", &__bss_start, &_end
printf "Kernel stack: %p-%p\n", &kernel_stack_start, &kernel_stack_end

# Analyze memory usage by subsystem
printf "Memory usage by subsystem:\n"
printf "Process table: %lu bytes\n", sizeof(mproc) * NR_PROCS
printf "Message buffers: %lu bytes\n", sizeof(mbuf) * NR_MBUFS
printf "Page tables: %lu bytes\n", sizeof(ptable) * NR_PTES
quit
EOF
}
```

## Integration with MINIX Build System

### Build Script Integration
```bash
# Integration with MINIX build process
post_build_actions() {
    local build_dir=$1
    local target=$2
    
    echo "Performing post-build validation for $target"
    
    # Validate kernel image
    if [ -f "$build_dir/kernel" ]; then
        echo "Validating kernel image..."
        riscv64-unknown-elf-readelf -h "$build_dir/kernel" > /dev/null
        if [ $? -eq 0 ]; then
            echo "Kernel image validation: PASSED"
        else
            echo "Kernel image validation: FAILED"
            return 1
        fi
    fi
    
    # Generate test script for this build
    generate_test_script "$build_dir" "$target"
    
    # Create debugging configuration
    create_debug_config "$build_dir" "$target"
}

generate_test_script() {
    local build_dir=$1
    local target=$2
    local test_script="$build_dir/test_${target}.sh"
    
    cat > "$test_script" << EOF
#!/bin/bash
# Auto-generated test script for $target

# Launch QEMU with built kernel
$(dirname $0)/qemu-riscv64.sh \
    -k "$build_dir/kernel" \
    -m 256M \
    -s
EOF
    
    chmod +x "$test_script"
    echo "Test script created: $test_script"
}
```

## Common Development Workflows

### Development Cycle Script
```bash
# Complete development cycle
minix_dev_cycle() {
    local action=$1
    
    case $action in
        build)
            echo "Building MINIX for RISC-V..."
            ./build.sh -m evbriscv64 -j$(nproc) distribution
            ;;
            
        test)
            echo "Testing MINIX..."
            run_kernel_tests basic 300
            ;;
            
        debug)
            echo "Starting debug session..."
            ./qemu-riscv64.sh -d &
            sleep 2
            ./gdb-riscv64.sh
            ;;
            
        analyze)
            echo "Analyzing performance..."
            analyze_performance 60
            ;;
            
        clean)
            echo "Cleaning build artifacts..."
            ./build.sh -c -m evbriscv64
            rm -f *.log *.core
            ;;
            
        *)
            echo "Usage: $0 {build|test|debug|analyze|clean}"
            ;;
    esac
}
```

### Automated Regression Testing
```bash
# Automated regression testing
run_regression_tests() {
    local test_cases=("ipc" "memory" "filesystem" "network")
    local results_file="regression_results.log"
    
    echo "Starting regression test suite: $(date)" > "$results_file"
    
    for test in "${test_cases[@]}"; do
        echo "Running $test tests..."
        
        # Start QEMU for test
        ./qemu-riscv64.sh -k kernel-test -m 256M > "${test}.log" 2>&1 &
        local qemu_pid=$!
        
        # Wait for test completion
        if wait_for_test_completion "$test" 300; then
            echo "$test: PASSED" >> "$results_file"
        else
            echo "$test: FAILED" >> "$results_file"
            echo "Check ${test}.log for details"
        fi
        
        kill $qemu_pid 2>/dev/null
    done
    
    echo "Regression testing complete: $(date)" >> "$results_file"
    cat "$results_file"
}
```

## Advanced Debugging Features

### Multi-Process Debugging
```bash
# Debug multiple MINIX services simultaneously
multi_service_debug() {
    local services=("pm" "vfs" "vm" "rs")
    local gdb_commands=""
    
    for service in "${services[@]}"; do
        gdb_commands+="printf \"Service $service:\\n\"; "
        gdb_commands+="info proc $service; "
        gdb_commands+="backtrace; "
    done
    
    riscv64-unknown-elf-gdb -batch \
        -ex "target remote :1234" \
        -ex "$gdb_commands" \
        -ex "quit"
}
```

### Memory Leak Detection
```bash
# Memory leak detection script
detect_memory_leaks() {
    local process_name=${1:-"kernel"}
    local duration=${2:-300}
    
    echo "Monitoring memory usage for $process_name over $duration seconds"
    
    # Sample memory usage over time
    for i in $(seq 1 $duration); do
        # Get memory statistics
        memory_stats=$(get_memory_stats "$process_name")
        echo "$(date): $memory_stats" >> memory_leak.log
        
        sleep 1
    done
    
    # Analyze for memory leaks
    analyze_memory_trend memory_leak.log
}
```

## Error Handling and Recovery

### Robust Script Error Handling
```bash
# Error handling in development scripts
set -euo pipefail

trap 'error_handler $LINENO' ERR

error_handler() {
    local line_no=$1
    echo "Error occurred at line $line_no"
    echo "Command: $BASH_COMMAND"
    echo "Exit code: $?"
    
    # Cleanup
    cleanup_resources
    
    # Generate error report
    generate_error_report
    
    exit 1
}

cleanup_resources() {
    # Kill any remaining QEMU processes
    pkill -f qemu-system-riscv64 || true
    
    # Clean up temporary files
    rm -f /tmp/gdb-minix-riscv64.*
    rm -f /tmp/qemu-minix-*.log
}
```

## Common Development Issues

### QEMU Launch Issues
- **Kernel not found**: Ensure kernel path is correct
- **Memory allocation failures**: Reduce memory size or check system resources
- **Device conflicts**: Verify no other QEMU instances are running
- **Network issues**: Check host network configuration

### GDB Debugging Issues
- **Connection refused**: Ensure QEMU is running in debug mode
- **Architecture mismatch**: Verify RISC-V GDB is installed
- **Symbol loading failures**: Check kernel symbols are available
- **Breakpoint issues**: Verify debug information in kernel

### Performance Analysis Issues
- **Insufficient sampling**: Increase monitoring duration
- **Data corruption**: Check log file integrity
- **Resource exhaustion**: Monitor system resources during analysis

## Future Enhancements

### Planned Script Features
- Automated performance regression detection
- Integrated code coverage analysis
- Advanced memory profiling tools
- Network traffic analysis
- Power consumption monitoring

### Development Tool Evolution
- GUI-based debugging interface
- Integrated development environment support
- Cloud-based testing infrastructure
- Continuous integration hooks
- Machine learning-based performance analysis

These development and debugging scripts represent essential tools for MINIX development, providing streamlined interfaces for complex development tasks while being specifically optimized for the unique requirements of microkernel development and RISC-V 64-bit platform support.