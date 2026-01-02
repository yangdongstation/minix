# MINIX 3 Operating System - Agent Development Guide

## Project Overview

MINIX 3 is a free, open-source microkernel-based operating system designed for high reliability and fault tolerance. This repository contains the complete MINIX 3 source code with active development focused on the RISC-V 64-bit architecture port.

### Key Characteristics

- **Microkernel Architecture**: Minimal kernel with most OS functionality in userspace servers
- **Message-Based IPC**: Inter-process communication replaces traditional system calls
- **Fault Tolerance**: Servers can crash and be restarted without affecting system stability
- **Multi-Architecture Support**: i386, ARM (earm), and RISC-V 64-bit (evbriscv64)
- **NetBSD Compatibility**: Shares utilities and libraries with NetBSD-8

### Current Development Focus

The project is actively developing RISC-V 64-bit support for the QEMU virt platform. The port is approximately 65% complete with core kernel infrastructure ready and ongoing work on user space integration.

## Technology Stack

### Core Technologies
- **Languages**: C (kernel, servers, drivers), Assembly (architecture-specific code)
- **Build System**: BSD Make with NetBSD-derived build infrastructure
- **Cross-Compilation**: GCC toolchains for target architectures
- **Emulation**: QEMU for development and testing
- **Version Control**: Git with Gerrit code review (`.gitreview`)

### Architecture Support
- **Primary Target**: RISC-V 64-bit (RV64GC) with QEMU virt platform
- **Memory Management**: Sv39 virtual memory (3-level page tables)
- **Boot Flow**: OpenSBI (M-mode) → MINIX kernel (S-mode) → User processes (U-mode)
- **Interrupt Handling**: SBI for software/timer interrupts, PLIC for external interrupts

## Project Structure

### Core MINIX Components (`minix/`)
```
minix/
├── kernel/           # Microkernel implementation
│   └── arch/         # Architecture-specific code (i386, earm, riscv64)
├── servers/          # Userspace system servers
│   ├── pm/          # Process manager
│   ├── vfs/         # Virtual file system
│   ├── vm/          # Virtual memory manager
│   ├── rs/          # Reincarnation server (crash recovery)
│   └── devman/      # Device manager
├── drivers/          # Hardware drivers
│   └── tty/ns16550/ # UART driver for RISC-V
├── lib/              # MINIX-specific libraries
├── commands/         # MINIX user utilities
└── tests/            # Test suite (including riscv64/)
```

### NetBSD Compatibility Layer
- **bin/, sbin/**: Core UNIX utilities synced with NetBSD-8
- **lib/**: Standard C library and support libraries
- **include/**: System headers and definitions
- **usr.bin/, usr.sbin/**: Additional user and system utilities

### Build Infrastructure
- **tools/**: Cross-compilation toolchain and host utilities
- **external/**: Third-party software organized by license
- **share/mk/**: BSD Make rules and build definitions
- **etc/**: System configuration and service definitions

## Build System

### Primary Build Script
The main entry point is `build.sh`, a NetBSD-derived build wrapper that handles cross-compilation and system building.

### Common Build Commands

```bash
# Build complete system for RISC-V 64-bit
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
./build.sh -j$(nproc) -m evbriscv64 \
  -V AVAILABLE_COMPILER=gcc -V ACTIVE_CC=gcc -V ACTIVE_CPP=gcc -V ACTIVE_CXX=gcc -V ACTIVE_OBJC=gcc \
  -V RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany' \
  -V NOGCCERROR=yes \
  -V MKPIC=no -V MKPICLIB=no -V MKPICINSTALL=no \
  -V MKCXX=no -V MKLIBSTDCXX=no -V MKATF=no \
  -V USE_PCI=no \
  -V CHECKFLIST_FLAGS='-m -e' \
  distribution

# Build only cross-compilation tools
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no ./build.sh -U -m evbriscv64 tools

# Clean and rebuild from scratch
./build.sh -c -m evbriscv64 distribution

# Run tests
./minix/tests/riscv64/run_tests.sh all
```

### Build Variables
Key environment variables for RISC-V builds:
- `MKPCI=no`: Skip PCI subsystem (not needed for QEMU virt)
- `HAVE_GOLD=no`: Disable gold linker (compatibility)
- `HAVE_LLVM=no MKLLVM=no`: Skip LLVM (fails on RISC-V)
- `HOST_CFLAGS="-O -fcommon"`: Host compiler flags
- `RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany'`: Fallback for toolchains that reject `-march=rv64gc`
- `CHECKFLIST_FLAGS='-m -e'`: Allow missing/extra files while sets are incomplete

### Cross-Compiler Requirements
- **Primary**: `riscv64-unknown-elf-gcc`
- **Alternatives**: `riscv64-linux-gnu-gcc`, `riscv64-elf-gcc`
- **Architecture Flags**: `-march=rv64gc -mabi=lp64d` (fallback: `-march=RV64IMAFD -mcmodel=medany`)

## Development Workflow

### Testing Infrastructure
```bash
# Run RISC-V specific tests
./minix/tests/riscv64/run_tests.sh [kernel|user|build|all]

# Validate build environment
./scripts/validate-build.sh

# Quick build check
./scripts/quick-check.sh
```

### QEMU Integration
```bash
# Run kernel in QEMU
./minix/scripts/qemu-riscv64.sh -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64

# Debug mode (wait for GDB)
./minix/scripts/qemu-riscv64.sh -d -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64

# Connect GDB debugger
./minix/scripts/gdb-riscv64.sh /path/to/kernel
```

### Service Configuration
System services are defined in `etc/system.conf` with permissions for:
- IPC targets (which servers they can communicate with)
- System calls (kernel functionality access)
- I/O ranges and IRQ assignments
- Memory management privileges

## Code Organization

### Message-Based Architecture
Instead of traditional system calls, MINIX uses message passing:
```c
message m;
m.m_type = READ;
m.VFS_READ = *read_params;
_sendrec(VFS_PROC_NR, &m);  // Synchronous message
```

### Server Communication Pattern
- **Synchronous**: `_sendrec()` for request-response
- **Asynchronous**: `send()`, `receive()` for non-blocking
- **Notifications**: `sendnb()`, `notify()` for events

### Driver Development
Drivers are userspace processes that:
1. Register with device manager (devman)
2. Receive I/O requests via messages
3. Use kernel syscalls for hardware access (`sys_inb`, `sys_irqmask`, etc.)

## RISC-V 64-bit Specifics

### Memory Map (QEMU virt)
| Address Range | Device | Description |
|---------------|--------|-------------|
| 0x02000000 | CLINT | Core Local Interruptor |
| 0x0C000000 | PLIC | Platform-Level Interrupt Controller |
| 0x10000000 | UART | NS16550 serial console |
| 0x10001000 | VirtIO | MMIO devices |
| 0x80000000 | RAM | Physical memory start |

### Key Architecture Files
- `minix/kernel/arch/riscv64/head.S`: Boot entry with Sv39 setup
- `minix/kernel/arch/riscv64/sbi.c`: SBI firmware interface
- `minix/kernel/arch/riscv64/plic.c`: Interrupt controller driver
- `minix/kernel/arch/riscv64/exception.c`: Trap/interrupt handling

### Testing Categories
- **csr**: Control and status registers
- **atomic**: Atomic operations
- **sbi**: SBI interface calls
- **memory**: Memory management
- **trap**: Exception handling
- **timer**: Timer functionality
- **ipc**: Inter-process communication
- **vm**: Virtual memory

## Development Guidelines

### Code Style
- Follow existing MINIX/NetBSD conventions
- Use tabs for indentation (8-space width)
- K&R brace style for functions
- Descriptive function and variable names

### Error Handling
- Always check return values from system calls
- Use MINIX's panic() for unrecoverable kernel errors
- Implement proper cleanup in drivers and servers

### Testing Requirements
- Add tests for new functionality in `minix/tests/`
- Run full test suite before submitting changes
- Include both positive and negative test cases

### Documentation
- Update relevant README files for architectural changes
- Document new system calls and messages
- Keep inline comments focused on "why" not "what"

## Common Issues and Solutions

### Build Failures
1. **LLVM compilation**: Use `HAVE_LLVM=no MKLLVM=no`
2. **Missing C++ directories**: Create manually during distribution
3. **Gold linker issues**: Use `HAVE_GOLD=no`
4. **Cross-compiler not found**: Check PATH and toolchain installation

### RISC-V Specific Issues
1. **SBI compatibility**: Ensure OpenSBI firmware version compatibility
2. **Memory alignment**: RISC-V requires strict alignment for some operations
3. **Atomic operations**: Use proper RISC-V atomic instructions

### Debugging Tips
1. Use QEMU's `-d` flag for debug output
2. Enable kernel debug prints with appropriate flags
3. Use GDB with hardware breakpoints for kernel debugging
4. Check `/proc/kmsg` for kernel messages

## Security Considerations

### Service Isolation
- Each service runs with minimal privileges
- IPC permissions control inter-service communication
- I/O and IRQ access is explicitly granted

### Memory Protection
- User processes cannot access kernel memory
- Services have controlled access to physical memory
- Virtual memory isolation between processes

### Code Review Process
- All changes go through Gerrit code review
- Automated testing validates builds
- Security implications must be considered for kernel changes

## Performance Optimization

### RISC-V Specific Optimizations
- Use compressed instructions (RVC) when possible
- Optimize memory barriers for weak memory ordering
- Leverage hardware performance counters

### System Tuning
- Adjust server priorities in `etc/system.conf`
- Configure quantum values for time-sharing
- Optimize message passing for high-frequency operations

## Build System Configuration Files

### Key Configuration Files
- **`build.sh`**: Main build wrapper script (NetBSD-derived)
- **`Makefile`**: Top-level build orchestration
- **`Makefile.inc`**: Common build definitions
- **`share/mk/bsd.own.mk`**: MINIX-specific build variables
- **`etc/system.conf`**: System service configuration
- **`etc/boot.cfg.default`**: Boot configuration

### Build Directory Structure
```
obj/
├── tooldir.*          # Cross-compilation tools
├── destdir.*          # Build output directory
└── releasedir         # Release artifacts
```

## Current Status and Metrics

### Development Status (as of December 2025)
- **Kernel Infrastructure**: ✅ Complete (100%)
- **Device Drivers**: ✅ UART complete, VirtIO in progress
- **Build System**: ✅ 90% functional
- **User Space**: 🔄 In progress (~65%)
- **Testing**: ✅ Comprehensive RISC-V test suite

### Quality Metrics
- **Code Coverage**: Kernel ~75%, Drivers ~60%
- **Build Success Rate**: 90% (some components still failing)
- **Test Pass Rate**: Architecture tests: 100%
- **Documentation**: 80% complete for implemented components

### Known Issues
1. **C++ Library Build**: Manual directory creation required
2. **LLVM Compilation**: Fails on RISC-V, must be disabled
3. **User Space Integration**: Ongoing development
4. **VirtIO Drivers**: Network and block device drivers in progress

## Development Environment Setup

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install gcc-riscv64-unknown-elf build-essential

# Alternative toolchains also work:
# - riscv64-linux-gnu-gcc
# - riscv64-elf-gcc
```

### Quick Start
```bash
# 1. Clone and setup
cd /path/to/minix

# 2. Build cross-compilation tools
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no ./build.sh -U -m evbriscv64 tools

# 3. Build complete system
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
./build.sh -j$(nproc) -m evbriscv64 \
  -V AVAILABLE_COMPILER=gcc -V ACTIVE_CC=gcc -V ACTIVE_CPP=gcc -V ACTIVE_CXX=gcc -V ACTIVE_OBJC=gcc \
  -V RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany' \
  -V NOGCCERROR=yes \
  -V MKPIC=no -V MKPICLIB=no -V MKPICINSTALL=no \
  -V MKCXX=no -V MKLIBSTDCXX=no -V MKATF=no \
  -V USE_PCI=no \
  -V CHECKFLIST_FLAGS='-m -e' \
  distribution

# 4. Run tests
./minix/tests/riscv64/run_tests.sh all

# 5. Test in QEMU
./minix/scripts/qemu-riscv64.sh -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64
```

This guide provides the essential information needed to understand, build, and develop the MINIX 3 operating system, with particular focus on the ongoing RISC-V 64-bit port development.
