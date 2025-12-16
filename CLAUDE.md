# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is MINIX 3, a free open-source operating system with a microkernel architecture. The codebase is derived from NetBSD but includes MINIX-specific components:

- **Microkernel design**: A minimal kernel with most OS functionality in userspace servers
- **IPC-based architecture**: Inter-process communication for all system calls and operations
- **Modular servers**: Separate servers for process management (pm), virtual memory (vm), file system (vfs), and more
- **Hybrid system**: Contains both NetBSD-derived utilities (bin, sbin) and MINIX-specific kernel and server code
- **Multi-architecture support**: Supports i386, ARM (earm), and RISC-V 64-bit architectures

## Build System

The project uses a make-based build system derived from NetBSD with `build.sh` as the primary entry point for building the entire system. The build system supports cross-compilation to multiple architectures.

### Common Build Commands

```bash
# Build the entire system (tools, kernel, libraries, utilities)
./build.sh

# Build for specific architecture (e.g., RISC-V 64-bit)
./build.sh -m evbriscv64

# Build cross-compilation tools only
./build.sh tools

# Build complete system distribution
./build.sh distribution

# Build in a specific DESTDIR (typically for creating a distribution)
./build.sh -D /tmp/minix-build

# Clean object directories and rebuild from scratch
./build.sh -c

# Build specific components
make -C minix build        # Build MINIX-specific components only
make -C lib build          # Build libraries only

# Run regression tests
make regression-tests

# Run MINIX-specific tests
./minix/tests/run

# Build with parallel jobs (faster on multi-core systems)
./build.sh -j$(nproc)
```

### Build Variables (in /etc/mk.conf or via environment)

- **DESTDIR**: Target directory for installation (defaults to /)
- **TOOLDIR**: Directory for host tools used during cross-compilation
- **MKMAN**: Set to "no" to skip building manual pages
- **MKSHARE**: Set to "no" to skip building /usr/share content
- **MKUPDATE**: Set to "yes" to skip cleandir during incremental builds
- **MKATF**: Set to "yes" to include automated testing framework tests
- **HAVE_GOLD**: Set to "no" to disable gold linker (sometimes needed for RISC-V)
- **HOST_CFLAGS**: Additional flags for host compiler during bootstrap
- **MKPCI**: Set to "no" to skip PCI subsystem (useful for embedded targets)

### Build Process Overview

1. **Tool Building**: `build.sh tools` builds cross-compilation tools in `obj/tooldir.*`
2. **Library Building**: Builds both MINIX-specific and NetBSD-derived libraries
3. **Component Building**: Builds kernel, servers, drivers, and utilities
4. **Distribution Creation**: `build.sh distribution` creates installable system sets

## Directory Structure

### Core System Components (minix/)

- **kernel/**: Microkernel implementation with architecture-specific code (arch/)
  - Responsible for process scheduling, interrupt handling, message passing
  - Architecture: Supports i386, earm (ARM), and riscv64 (RISC-V 64-bit)
  - Key files: clock.c, interrupt.c, debug.c

- **servers/**: Userspace servers that provide core OS functionality
  - **pm/**: Process manager - handles process creation, signals, exit
  - **vfs/**: Virtual file system - file operations, mounting, path resolution
  - **vm/**: Virtual memory manager - memory allocation, paging, page replacement
  - **rs/**: Reincarnation server - crash recovery and service restart
  - **ds/**: Data store - dynamic system property storage
  - **is/**: Information server - system statistics and debugging
  - **sched/**: Process scheduler
  - **devman/**: Device manager - hardware device management
  - **ipc/**: Inter-process communication utilities

- **drivers/**: Hardware drivers for disks, network, etc.

- **lib/**: MINIX-specific libraries
  - libsys: System call stubs
  - libc: C standard library
  - Various driver and system libraries

- **commands/**: MINIX user utilities (different from traditional bin/)

- **usr.bin/ and usr.sbin/**: Additional utilities

- **tests/**: Automated test suite
  - Architecture tests
  - Block device tests
  - Data store tests
  - Common socket tests
  - Kernel tests
  - Run tests with: `./minix/tests/run`

### NetBSD Compatibility (also used in MINIX)

- **bin/, sbin/**: Core UNIX utilities (synced with NetBSD-8)
- **lib/**: Standard libraries
- **include/**: System headers
- **etc/**: Configuration files (protocols, services, etc.)
- **share/**: Man pages and data files
- **games/**: Game utilities
- **tools/**: Host tools for cross-compilation (gcc, clang, make, binutils, etc.)

## Key Architecture Concepts

### Message-Based IPC
Instead of traditional system calls, MINIX uses message passing between the kernel and servers. Understanding how messages are structured and routed is crucial for system modifications.

### Process Manager (pm) to VFS to VM Interaction
These three servers coordinate for most OS operations:
- Process creation goes through pm, which coordinates with vfs and vm
- File operations use vfs, which may trigger vm operations
- Memory operations are handled by vm

### Server Restart Mechanism
The Reincarnation Server (rs) can detect server crashes and restart them. Services define their restart policies and dependencies.

### Device Drivers
Drivers are typically standalone processes communicating with the kernel via special syscalls for hardware access (interrupts, I/O ports, memory access).

## Testing

### Run MINIX Tests
```bash
cd minix/tests
./run               # Run all tests
./run <test_name>   # Run specific test

# Run architecture-specific tests
./minix/tests/riscv64/run_tests.sh all      # Run all RISC-V tests
./minix/tests/riscv64/run_tests.sh kernel   # Run kernel tests only
./minix/tests/riscv64/run_tests.sh user     # Run user-space tests only
./minix/tests/riscv64/run_tests.sh build    # Run build system tests
```

### Common Test Categories
- **blocktest**: Block device functionality
- **ddekit**: Device driver kit tests
- **ds**: Data store tests
- **kernel**: Low-level kernel tests
- **rmibtest**: RMIBD interface tests
- **fbdtest**: Fault-tolerant block device tests
- **common.c/.h**: Shared test utilities

## Building for MINIX Specifically

The main Makefile includes conditional logic for `__MINIX` to include MINIX-specific components:

```makefile
.if defined(__MINIX)
  # MINIX-specific build rules
.endif
```

This is set during the build process and ensures MINIX-specific servers and kernel are built while bypassing NetBSD-only components.

## Important Files and Configuration

- **Makefile**: Top-level build orchestration
- **build.sh**: Main build script (handles tool building, cross-compilation setup)
- **tools/Makefile**: Host tool building
- **minix/Makefile**: MINIX component building
- **share/mk/bsd.*.mk**: Make rules for building programs, libraries, etc. (from NetBSD)
- **etc/mk.conf**: Build configuration file

## Recent Development Focus

Recent commits show active development on multiple fronts:
- RISC-V 64-bit port progress (stage6 buildable, GAS compiling)
- Synchronization with NetBSD-8 for utilities and manuals
- Removal of NOCRYPTO build option
- Updates to standard tools and protocols files
- Focus on standards compliance and bug fixes

## Common Development Patterns

### Message Passing System Calls
System calls in MINIX are implemented as messages to appropriate servers:
```c
// Example: File read operation sends message to VFS
message m;
m.m_type = READ;
m.VFS_READ = *read_params;
_sendrec(VFS_PROC_NR, &m);
```

### Driver Development
Drivers are userspace processes that:
1. Register with the device manager (devman)
2. Receive I/O requests via messages
3. Use kernel syscalls for hardware access (sys_inb, sys_outb, sys_irqmask)

### Server Communication
Servers communicate using:
- Synchronous messaging: `_sendrec()`
- Asynchronous messaging: `send()`, `receive()`
- Notification: `sendnb()`, `notify()`

## Development Notes

1. **MINIX-specific code**: Look in `minix/` directory; NetBSD-derived code in other directories
2. **System calls**: Implemented as messages sent to pm, vfs, or vm servers
3. **Testing**: Use ATF (Automated Test Framework) for new tests
4. **Build dependencies**: Always build tools first, then libraries, then applications
5. **Portability**: Tools in tools/ must be highly portable (C89 compatible where possible)

## RISC-V 64-bit Port

The RISC-V 64-bit port is a recent addition targeting the QEMU virt platform. The port is currently in active development (see recent commits: "stage6 buildable", "GAS is now compiling successfully!").

### Build Commands
```bash
# Build cross-compilation tools for RISC-V 64-bit
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no ./build.sh -m evbriscv64 tools

# Build complete distribution for RISC-V 64-bit
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
./build.sh -U -m evbriscv64 distribution

# Check if architecture is recognized
./build.sh -m evbriscv64 list-arch

# Build with parallel jobs (faster on multi-core systems)
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
./build.sh -j$(nproc) -m evbriscv64 distribution

# Clean and rebuild from scratch
./build.sh -c -m evbriscv64
```

### Known Build Issues & Solutions

#### LLVM/Clang Issues
- Use `HAVE_LLVM=no MKLLVM=no` to skip LLVM build (required for RISC-V)
- LLVM compilation fails on RISC-V 64-bit architecture

#### Missing Architecture Files
The following files are needed for complete RISC-V 64-bit support:
- `minix/lib/libminc/arch/riscv64/Makefile.libc.inc`
- `minix/lib/libminc/arch/riscv64/sys/Makefile.inc`
- `minix/tests/arch/riscv64/Makefile.inc`
- Various driver Makefile.inc files in `minix/drivers/*/arch/riscv64/`

#### C++ Library Directories
Manual creation required during distribution build:
```bash
mkdir -p $DESTDIR/usr/include/g++/bits/riscv32
mkdir -p $DESTDIR/usr/include/g++/bits/riscv64
```

### Running with QEMU
```bash
# Basic run with pre-built kernel
./minix/scripts/qemu-riscv64.sh -k /path/to/kernel

# Debug mode with GDB server
./minix/scripts/qemu-riscv64.sh -d -k /path/to/kernel

# Connect GDB client to debug session
./minix/scripts/gdb-riscv64.sh /path/to/kernel

# Run with specific memory and CPU configuration
./minix/scripts/qemu-riscv64.sh -k /path/to/kernel -m 512M -smp 4
```

### RISC-V Architecture Files
- **minix/kernel/arch/riscv64/**: Kernel architecture support
  - `sbi.c`: SBI (Supervisor Binary Interface) firmware interface for boot/time/console
  - `plic.c`: Platform-Level Interrupt Controller (with `plic_irq_cpu_mask()` for SMP support)
  - `head.S`: Boot entry point with Sv39 page table setup and initial trap handling
  - `exception.c`: Trap/interrupt handling and system call dispatch
  - `arch_clock.c`: Timer and clock management using CLINT
  - `memory.c`: Memory management and page table operations
- **minix/lib/libc/arch/riscv64/**: C library architecture-specific stubs
- **minix/lib/libsys/arch/riscv64/**: System library architecture-specific code
- **minix/drivers/storage/virtio_blk_mmio/**: VirtIO block device driver for MMIO
- **minix/drivers/tty/ns16550/**: UART driver for QEMU's NS16550 serial port

### Memory Map (QEMU virt platform)
| Address      | Device           | Description                   |
|--------------|------------------|-------------------------------|
| 0x02000000   | CLINT            | Core Local Interrupter (timer)|
| 0x0C000000   | PLIC             | Platform-Level Interrupt Ctrl |
| 0x10000000   | UART             | NS16550 serial port           |
| 0x10001000   | VirtIO MMIO      | VirtIO devices (block, net)   |
| 0x80000000   | RAM start        | Physical memory start         |

### RISC-V Specific Tests
The RISC-V port includes comprehensive test coverage:
```bash
# Run all RISC-V specific tests
./minix/tests/riscv64/run_tests.sh all

# Individual test categories:
./minix/tests/riscv64/run_tests.sh kernel   # Kernel boot and basic functionality
./minix/tests/riscv64/run_tests.sh user     # User-space compilation tests
./minix/tests/riscv64/run_tests.sh build    # Build system validation
```

The test runner automatically detects and tests:
- Kernel boot with QEMU
- SMP initialization (when implemented)
- Timer interrupt functionality
- Cross-compiler availability
- Architecture-specific source files
- VirtIO driver presence

### Cross-Compiler Requirements

The RISC-V 64-bit port requires a GCC cross-compiler toolchain:
```bash
# Ubuntu/Debian installation
sudo apt-get install gcc-riscv64-unknown-elf

# Alternative toolchains that work:
- riscv64-linux-gnu-gcc
- riscv64-elf-gcc

# Compiler flags used:
- -march=rv64gc    # RISC-V 64-bit with G and C extensions
- -mabi=lp64d      # LP64 data model with double-precision floating point
```
