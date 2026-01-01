# Architecture-Specific System Call Library

## Overview

The `libsys/arch/` directory contains architecture-specific implementations of low-level system calls and hardware abstraction functions for MINIX 3. This directory provides the critical interface between the MINIX system libraries and the underlying hardware architecture, implementing functions that require direct hardware access or architecture-specific optimizations.

## Purpose and Responsibilities

### Core Functions
- **Hardware Abstraction**: Provides architecture-independent interfaces to hardware-specific operations
- **System Call Implementation**: Implements low-level system calls requiring privileged operations
- **Memory Management**: Handles architecture-specific memory operations and mappings
- **Device I/O**: Implements hardware device input/output operations
- **Performance Monitoring**: Provides access to hardware performance counters
- **Atomic Operations**: Implements architecture-specific atomic operations

### Key Features
- Architecture-specific optimizations for performance
- Hardware abstraction for portability
- Direct hardware access when necessary
- Efficient atomic operation implementations
- Platform-specific debugging support

## Directory Structure

```
minix/lib/libsys/arch/
├── i386/               # Intel x86 architecture support
├── earm/               # ARM architecture support  
└── riscv64/            # RISC-V 64-bit architecture support
```

## RISC-V 64-bit Implementation (`riscv64/`)

### Core Files
```
riscv64/
├── getidle.c           # CPU idle time measurement
├── get_randomness.c    # Hardware random number generation
├── ser_putc.c          # Serial console output
├── spin.c              # Spinlock implementation
├── sys_sdevio.c        # System device I/O operations
├── sys_umap_remote.c   # Remote memory mapping
├── tsc_util.c          # Timer/performance counter utilities
└── Makefile.inc        # Build configuration
```

### Device I/O Implementation (`sys_sdevio.c`)
Implements memory-mapped I/O operations for RISC-V:

```c
/* RISC-V uses memory-mapped I/O, not port I/O */
int sys_insb(u32_t port, endpoint_t ep, void *buf, int count)
{
    volatile u8_t *mmio = (volatile u8_t *)(uintptr_t)port;
    u8_t *p = (u8_t *)buf;
    int i;
    
    for (i = 0; i < count; i++) {
        p[i] = *mmio;
    }
    
    return OK;
}
```

**Key Features:**
- Memory-mapped I/O simulation for compatibility
- Support for byte, word, and dword operations
- Direct hardware register access
- Compatibility with MINIX driver interface

### Serial Console Support (`ser_putc.c`)
Provides low-level serial output for debugging:
- Direct UART register access
- NS16550-compatible UART support
- Platform-specific console initialization
- Interrupt-safe output operations

### Spinlock Implementation (`spin.c`)
Architecture-specific spinlock primitives:
```c
/* RISC-V atomic operations for spinlocks */
void spin_lock(spinlock_t *lock)
{
    while (__atomic_test_and_set(&lock->s, __ATOMIC_ACQUIRE)) {
        while (lock->s) {
            /* RISC-V pause instruction or equivalent */
            __asm__ volatile ("pause" ::: "memory");
        }
    }
}
```

### Performance Monitoring (`tsc_util.c`)
Hardware performance counter access:
- RISC-V cycle counter (mcycle) access
- Instruction counter (minstret) support
- Performance profiling interfaces
- Time measurement utilities

### Random Number Generation (`get_randomness.c`)
Hardware entropy collection:
- Platform-specific entropy sources
- RISC-V hardware random number generators
- Entropy pool management
- Cryptographic random number support

### Memory Mapping (`sys_umap_remote.c`)
Remote memory access support:
- Cross-process memory mapping
- Hardware address translation
- Memory protection handling
- Architecture-specific page table access

### CPU Idle Measurement (`getidle.c`)
System idle time calculation:
- CPU utilization measurement
- Idle task monitoring
- Performance statistics collection
- Power management integration

## Architecture-Specific Considerations

### RISC-V Memory Model
- Weak memory ordering requirements
- Memory barrier implementations
- Atomic operation primitives
- Cache coherence considerations

### Privilege Levels
- Machine-mode (M-mode) operations
- Supervisor-mode (S-mode) interfaces
- User-mode (U-mode) system calls
- SBI (Supervisor Binary Interface) usage

### Platform Integration
- QEMU virt platform specifics
- Hardware device addressing
- Interrupt controller interfaces
- Timer and clock source handling

## x86 Implementation (`i386/`)

### Legacy Support
- Port I/O instruction support (inb/outb)
- x86-specific atomic operations
- Performance counter access (RDTSC)
- Memory segmentation handling
- Legacy device compatibility

### Key Functions
- Port-mapped I/O operations
- Model-specific register access
- CPUID instruction support
- x86 memory management
- Hardware debugging support

## ARM Implementation (`earm/`)

### ARM-Specific Features
- ARM atomic instruction support
- Memory barrier implementations
- ARM performance monitoring
- Platform-specific optimizations
- Thumb mode compatibility

### Embedded Support
- ARM embedded platform support
- Power-efficient implementations
- Real-time extensions
- Hardware abstraction layer

## Build System Integration

### Makefile Configuration
Each architecture directory contains `Makefile.inc` with:
- Architecture-specific compiler flags
- Source file selection
- Optimization settings
- Platform-specific definitions

### Cross-Compilation Support
- Architecture detection during build
- Toolchain configuration
- Platform-specific linking
- Conditional compilation based on target

## System Integration

### Library Interface
The architecture-specific code provides implementations for:
- System call stubs
- Hardware abstraction functions
- Performance monitoring
- Debugging support
- Platform initialization

### Kernel Coordination
- System call vector setup
- Hardware feature detection
- Platform-specific initialization
- Exception handling setup
- Memory management coordination

## Performance Optimizations

### Architecture-Specific Optimizations
- Inline assembly for critical paths
- Hardware-specific instruction usage
- Cache-aware implementations
- Atomic operation optimizations
- Memory access optimizations

### RISC-V Optimizations
- Compressed instruction (RVC) usage
- Hardware performance counter utilization
- Efficient memory barrier usage
- Platform-specific tuning

## Testing and Validation

### Cross-Platform Testing
- Architecture-specific test suites
- Hardware abstraction validation
- Performance benchmarking
- Compatibility testing
- Regression testing

### Hardware Verification
- Real hardware testing
- Emulation platform validation
- Edge case handling
- Error condition testing
- Performance profiling

## Debugging Support

### Architecture-Specific Debugging
- Hardware debugging interface access
- Performance counter monitoring
- Memory inspection utilities
- Register state access
- Platform-specific debug features

### Development Tools
- Architecture-aware debugging
- Performance analysis tools
- Memory debugging support
- Hardware tracing integration
- Platform-specific utilities

## Future Enhancements

### RISC-V Evolution
- Support for future RISC-V extensions
- Enhanced performance monitoring
- Advanced atomic operations
- Platform-specific optimizations

### Multi-Architecture Support
- New architecture porting framework
- Enhanced hardware abstraction
- Performance portability
- Cross-platform optimization

The architecture-specific system call library provides the essential hardware abstraction layer that enables MINIX 3 to run efficiently across different processor architectures while maintaining system portability and performance.