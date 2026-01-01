# RISC-V 64-bit Test Suite Architecture

## Overview

The `minix/tests/riscv64/` directory contains a comprehensive test suite specifically designed for validating RISC-V 64-bit architecture support in MINIX 3. This test suite focuses on RISC-V-specific functionality including control and status registers, atomic operations, memory management, interrupt handling, and platform-specific features.

## Purpose and Responsibilities

### Core Functions
- **Architecture Validation**: Tests RISC-V 64-bit specific instructions and features
- **Hardware Feature Detection**: Validates CPU capabilities and extensions
- **Platform Integration**: Tests RISC-V platform-specific functionality
- **Performance Verification**: Benchmarks RISC-V-specific optimizations
- **Compatibility Testing**: Ensures compliance with RISC-V specifications
- **Error Handling**: Validates exception and interrupt handling

### Key Features
- Comprehensive RISC-V ISA coverage
- Platform-specific test validation
- Automated test execution and reporting
- Performance benchmarking and analysis
- Hardware capability detection
- Specification compliance verification

## Directory Structure

```
minix/tests/riscv64/
├── run_tests.sh        # Automated test runner script
├── Makefile            # Build configuration
├── test_csr.c          # Control and Status Register tests
├── test_atomic.c       # Atomic operation tests
├── test_sbi.c          # Supervisor Binary Interface tests
├── test_memory.c       # Memory management tests
├── test_trap.c         # Exception and interrupt tests
├── test_timer.c        # Timer and clock tests
├── test_ipc.c          # Inter-process communication tests
└── test_vm.c           # Virtual memory tests
```

## Test Categories

### Control and Status Register Tests (`test_csr.c`)
Comprehensive CSR validation:
```c
/* CSR read/write validation */
test_csr_basic(void)
{
    unsigned long mstatus, original;
    
    /* Save original value */
    original = read_csr(mstatus);
    
    /* Test read-modify-write */
    write_csr(mstatus, original | MSTATUS_MIE);
    mstatus = read_csr(mstatus);
    
    /* Verify modification */
    assert(mstatus & MSTATUS_MIE);
}
```

**CSR Coverage:**
- Machine status registers (mstatus, mie, mip)
- Supervisor status registers (sstatus, sie, sip)
- User status registers (ustatus)
- Timer registers (mtime, mtimecmp)
- Performance counters
- Hardware capability registers

### Atomic Operation Tests (`test_atomic.c`)
RISC-V atomic instruction validation:
- Load-reserved/store-conditional (LR/SC)
- Atomic memory operations (AMO)
- Compare-and-swap operations
- Fetch-and-add operations
- Memory ordering validation
- Multi-processor synchronization

### SBI Tests (`test_sbi.c`)
Supervisor Binary Interface testing:
```c
/* SBI console output test */
test_sbi_console(void)
{
    const char *msg = "RISC-V SBI Test\n";
    
    /* Test SBI console putchar */
    for (const char *p = msg; *p; p++) {
        sbi_console_putchar(*p);
    }
}
```

**SBI Coverage:**
- Console input/output operations
- System shutdown and reset
- Timer programming
- Inter-processor interrupts
- Memory fence operations
- Hardware performance counters

### Memory Management Tests (`test_memory.c`)
RISC-V memory system validation:
- Virtual memory translation
- Page table operations
- Memory protection mechanisms
- Cache operations
- TLB management
- Memory barrier effectiveness

### Exception and Interrupt Tests (`test_trap.c`)
RISC-V exception handling validation:
- Exception vector setup
- Trap handler installation
- Interrupt enable/disable
- Exception priority testing
- Nested exception handling
- Context save/restore

### Timer Tests (`test_timer.c`)
RISC-V timer functionality:
- Machine timer programming
- Supervisor timer access
- Timer interrupt generation
- Timer accuracy measurement
- Clock source validation
- Tick rate testing

### IPC Tests (`test_ipc.c`)
Inter-process communication on RISC-V:
- Message passing performance
- Synchronization primitive testing
- Shared memory operations
- Signal handling
- Process synchronization
- Cross-process communication

### Virtual Memory Tests (`test_vm.c`)
RISC-V virtual memory system:
- Sv39 page table validation
- Address translation testing
- Memory mapping operations
- Copy-on-write implementation
- Page fault handling
- Memory protection enforcement

## Test Execution Framework

### Automated Test Runner (`run_tests.sh`)
Comprehensive test execution system:
```bash
#!/bin/bash
# RISC-V test runner with categories
run_category() {
    local category=$1
    echo "Running $category tests..."
    
    case $category in
        "csr")
            run_csr_tests
            ;;
        "atomic")
            run_atomic_tests
            ;;
        "all")
            run_all_tests
            ;;
    esac
}
```

**Execution Modes:**
- Individual test category execution
- Complete test suite run
- Performance benchmarking mode
- Debug and verbose output
- Result aggregation and reporting

### Test Categories
- **csr**: Control and Status Register tests
- **atomic**: Atomic operation tests
- **sbi**: Supervisor Binary Interface tests
- **memory**: Memory management tests
- **trap**: Exception and interrupt tests
- **timer**: Timer functionality tests
- **ipc**: Inter-process communication tests
- **vm**: Virtual memory tests
- **all**: Complete test suite

## RISC-V Specific Features

### Hardware Detection
Automatic RISC-V feature detection:
```c
/* RISC-V hardware capability detection */
detect_riscv_features(void)
{
    unsigned long misa = read_csr(misa);
    
    /* Check for RV64 */
    if ((misa & MISA_MXL) == MISA_MXL_64) {
        printf("RISC-V 64-bit detected\n");
    }
    
    /* Check for extensions */
    if (misa & MISA_A) printf("Atomic extension present\n");
    if (misa & MISA_C) printf("Compressed extension present\n");
    if (misa & MISA_M) printf("Multiply extension present\n");
}
```

### Platform-Specific Testing
QEMU virt platform validation:
- VirtIO device testing
- Platform interrupt controller
- Memory-mapped I/O validation
- Timer accuracy verification
- Console functionality testing

### Performance Benchmarking
RISC-V performance measurement:
- Instruction throughput
- Memory access latency
- Atomic operation performance
- Context switch timing
- System call overhead

## Test Validation Framework

### Result Verification
Comprehensive validation approach:
- Expected vs actual comparison
- Hardware specification compliance
- Performance baseline establishment
- Error condition validation
- Cross-platform consistency

### Error Handling
Robust error detection and reporting:
- Hardware feature availability checking
- Privilege level validation
- Resource availability verification
- Graceful degradation support
- Detailed error diagnostics

## Integration with Build System

### Automated Testing
Build-time test integration:
- Cross-compilation support
- Architecture-specific compilation
- Test dependency management
- Result integration with build
- Failure notification

### Platform Support
Multi-platform testing:
- QEMU virt platform
- Real hardware testing
- SiFive platform support
- Generic RISC-V validation
- Future platform extensibility

## Performance Analysis

### Benchmark Integration
Performance measurement and analysis:
- Baseline performance establishment
- Optimization validation
- Regression detection
- Comparative analysis
- Scalability testing

### Profiling Support
Test execution profiling:
- Time measurement accuracy
- Resource utilization tracking
- Bottleneck identification
- Optimization opportunity detection
- Performance trend analysis

## Debugging and Diagnostics

### Hardware Debugging
RISC-V debugging support:
- Hardware breakpoint utilization
- Performance counter access
- Register state inspection
- Memory content analysis
- Execution trace support

### Software Diagnostics
Comprehensive diagnostic support:
- Detailed failure reporting
- System state capture
- Execution path tracking
- Resource usage monitoring
- Error pattern analysis

## Quality Assurance

### Specification Compliance
RISC-V specification validation:
- ISA compliance verification
- Privilege specification adherence
- Platform specification validation
- Extension compatibility testing
- Standard compliance certification

### Regression Testing
Long-term stability validation:
- Historical result comparison
- Change impact analysis
- Stability trend monitoring
- Performance regression detection
- Compatibility preservation

## Future Enhancements

### RISC-V Evolution
Support for emerging RISC-V features:
- New ISA extensions
- Advanced platform features
- Enhanced performance monitoring
- Security extension support
- Virtualization capabilities

### Testing Innovation
Advanced testing methodologies:
- Formal verification integration
- Automated test generation
- Machine learning-based analysis
- Predictive failure detection
- Advanced profiling techniques

The RISC-V 64-bit test suite provides comprehensive validation of RISC-V architecture support in MINIX 3, ensuring reliable operation across different RISC-V platforms while maintaining specification compliance and optimal performance.