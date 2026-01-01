# MINIX System Architecture Headers - Core Architecture Support

## Overview

The `sys/arch/` directory serves as the central hub for architecture-specific system definitions in MINIX 3. This directory contains the organizational framework for all supported processor architectures, providing the essential infrastructure that enables MINIX to run on diverse hardware platforms while maintaining a consistent system interface.

## Purpose and Responsibilities

### Core Functions
- **Architecture Abstraction**: Provides a unified interface for different processor architectures
- **Platform Organization**: Manages architecture-specific directories and build configurations
- **Cross-Platform Support**: Enables MINIX to support multiple processor families
- **Build System Integration**: Coordinates architecture-specific builds and header installations
- **Hardware Interface Standardization**: Defines common patterns for architecture-specific code

### Supported Architectures
- **RISC-V 64-bit** (`evbriscv64/`) - Primary development target
- **Intel x86** (`i386/`) - Traditional MINIX platform
- **ARM** (`arm/`, `evbarm/`) - Embedded systems support
- **Generic RISC-V** (`riscv/`) - Base RISC-V definitions
- **x86 Extensions** (`x86/`) - Extended x86 family support

## Directory Structure

```
sys/arch/
├── Makefile              # Main architecture makefile
├── README               # Architecture support matrix
├── arm/                 # ARM architecture support
├── evbarm/              # ARM evaluation boards
├── evbriscv64/          # RISC-V 64-bit evaluation boards
├── i386/                # Intel x86 architecture
├── riscv/               # Generic RISC-V support
└── x86/                 # Extended x86 architecture
```

## Architecture Integration Framework

### Build System Architecture
The arch directory implements a hierarchical build system that:
- Automatically detects target architectures
- Configures cross-compilation toolchains
- Manages architecture-specific dependencies
- Coordinates header file installations
- Handles platform-specific optimizations

### Architecture Selection Mechanism
Architecture selection is handled through:
```makefile
# Architecture detection and configuration
MACHINE_ARCH = $(shell uname -m | sed 's/i.86/i386/')
MACHINE = $(MACHINE_ARCH)

# Architecture-specific includes
.include "${ARCHDIR}/Makefile.inc"
```

## Architecture-Specific Components

### Header File Organization
Each architecture directory provides:
- **CPU Definitions** (`include/arch/cpu.h`) - Processor-specific constants
- **Memory Management** (`include/arch/pmap.h`) - Physical memory mapping
- **Interrupt Handling** (`include/arch/intr.h`) - Interrupt controller interfaces
- **Register Definitions** (`include/arch/reg.h`) - Hardware register layouts
- **System Parameters** (`include/arch/param.h`) - Architecture parameters

### Platform-Specific Features
Architecture directories contain:
- **Boot Support** - Platform initialization code
- **Device Drivers** - Architecture-specific hardware interfaces
- **System Calls** - Platform-specific system call conventions
- **Binary Formats** - Executable file format support
- **Performance Counters** - Hardware performance monitoring

## RISC-V 64-bit Architecture Support

### Primary Development Focus
The `evbriscv64/` directory represents the primary development target:
- **Modern 64-bit RISC-V** with RV64GC instruction set
- **QEMU virt platform** for development and testing
- **Sv39 virtual memory** with 3-level page tables
- **SBI integration** for firmware interface
- **PLIC support** for interrupt handling

### RISC-V Architecture Integration
```c
/* Architecture-specific RISC-V integration */
#ifdef __riscv64__
#include "arch/evbriscv64/include/arch/param.h"
#include "arch/evbriscv64/include/arch/vmparam.h"
#include "arch/evbriscv64/include/arch/trap.h"
#endif
```

## Cross-Architecture Compatibility

### Common Interface Patterns
All architectures provide consistent interfaces for:
- **Memory Management** - Virtual and physical memory operations
- **Process Management** - Context switching and scheduling
- **Device Access** - I/O port and memory-mapped device access
- **Interrupt Handling** - Interrupt registration and handling
- **System Calls** - User-to-kernel transition mechanisms

### Architecture Abstraction Layer
The arch directory implements abstraction patterns:
```c
/* Architecture-neutral interfaces */
struct arch_frame {
    /* Architecture-specific register state */
    union {
#ifdef __i386__
        struct i386_frame i386;
#endif
#ifdef __riscv64__
        struct riscv64_frame riscv64;
#endif
#ifdef __arm__
        struct arm_frame arm;
#endif
    } u;
};
```

## Build System Integration

### Cross-Compilation Support
The architecture system supports multiple cross-compilation scenarios:
```makefile
# Cross-compilation toolchain configuration
TOOLDIR = ${OBJDIR}/tools
CROSS_COMPILE = ${TOOLDIR}/bin/${MACHINE_ARCH}--netbsd-

# Architecture-specific compiler flags
ARCH_CFLAGS = -march=rv64gc -mabi=lp64d  # RISC-V 64-bit
ARCH_CFLAGS = -m32                       # i386
ARCH_CFLAGS = -marm                      # ARM
```

### Header Installation Process
Architecture headers are installed systematically:
1. **Source Headers** - Located in `sys/arch/*/include/`
2. **Build Process** - Copied to object directory during build
3. **System Integration** - Made available to kernel and user programs
4. **Cross-Reference Validation** - Ensures consistency across architectures

## Memory Model Support

### Architecture-Specific Memory Models
Each architecture implements its memory model:
- **RISC-V** - Weak memory ordering with explicit barriers
- **x86** - Strong memory ordering with implicit barriers
- **ARM** - Configurable memory ordering based on architecture version

### Virtual Memory Integration
Architecture support for virtual memory:
```c
/* Architecture-specific virtual memory parameters */
#ifdef __riscv64__
#define ARCH_VM_MAXUSERADDR 0x0000003FFFFFFFFFUL
#define ARCH_VM_MAXADDR     0xFFFFFFFFFFFFFFFFUL
#endif
```

## Performance and Optimization

### Architecture-Specific Optimizations
The arch directory enables platform-specific optimizations:
- **Instruction Selection** - Use of architecture-specific instructions
- **Memory Alignment** - Platform-specific alignment requirements
- **Cache Management** - Architecture-specific cache operations
- **Atomic Operations** - Hardware-specific atomic primitives

### Performance Monitoring
Architecture-specific performance monitoring:
- **Hardware Counters** - CPU-specific performance counters
- **Profiling Support** - Platform-specific profiling interfaces
- **Debugging Features** - Architecture-specific debugging support

## Testing and Validation

### Architecture Compatibility Testing
Comprehensive testing ensures:
- **API Consistency** - Common interfaces work across architectures
- **ABI Compliance** - Application binary interface compatibility
- **Performance Validation** - Architecture-specific optimizations work correctly
- **Cross-Platform Builds** - Code builds successfully on all supported platforms

### Platform-Specific Testing
Each architecture undergoes:
- **Unit Testing** - Individual component validation
- **Integration Testing** - System-level functionality testing
- **Performance Testing** - Architecture-specific performance validation
- **Compatibility Testing** - Third-party software compatibility

## Future Architecture Support

### Emerging Architecture Support
The arch directory framework supports:
- **New Processor Families** - Easy addition of new architectures
- **Architecture Evolution** - Support for new instruction set extensions
- **Platform Variants** - Multiple platforms per architecture family
- **Hybrid Architectures** - Support for heterogeneous computing systems

### Extensibility Features
Framework supports future enhancements:
- **Modular Architecture** - Easy addition of new architectures
- **Configuration Management** - Flexible platform configuration
- **Version Management** - Support for architecture variants
- **Documentation Standards** - Consistent documentation across architectures

The `sys/arch/` directory provides the essential foundation that enables MINIX 3 to support multiple processor architectures while maintaining a consistent and reliable system interface. This architecture abstraction layer is crucial for MINIX's portability and enables the operating system to adapt to new hardware platforms efficiently.