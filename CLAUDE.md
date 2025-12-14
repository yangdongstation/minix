# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is MINIX 3, a free open-source operating system with a microkernel architecture. The codebase is derived from NetBSD but includes MINIX-specific components:

- **Microkernel design**: A minimal kernel with most OS functionality in userspace servers
- **IPC-based architecture**: Inter-process communication for all system calls and operations
- **Modular servers**: Separate servers for process management (pm), virtual memory (vm), file system (vfs), and more
- **Hybrid system**: Contains both NetBSD-derived utilities (bin, sbin) and MINIX-specific kernel and server code

## Build System

The project uses a make-based build system with `build.sh` as the primary entry point for building the entire system.

### Common Build Commands

```bash
# Build the entire system (tools, kernel, libraries, utilities)
./build.sh

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
```

### Build Variables (in /etc/mk.conf or via environment)

- **DESTDIR**: Target directory for installation (defaults to /)
- **TOOLDIR**: Directory for host tools used during cross-compilation
- **MKMAN**: Set to "no" to skip building manual pages
- **MKSHARE**: Set to "no" to skip building /usr/share content
- **MKUPDATE**: Set to "yes" to skip cleandir during incremental builds
- **MKATF**: Set to "yes" to include automated testing framework tests

## Directory Structure

### Core System Components (minix/)

- **kernel/**: Microkernel implementation with architecture-specific code (arch/)
  - Responsible for process scheduling, interrupt handling, message passing
  - Architecture: Currently supports i386 and earm (ARM)
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

Recent commits show:
- Synchronization with NetBSD-8 for utilities and manuals
- Removal of NOCRYPTO build option
- Updates to standard tools and protocols files
- Focus on standards compliance and bug fixes

## Development Notes

1. **MINIX-specific code**: Look in `minix/` directory; NetBSD-derived code in other directories
2. **System calls**: Implemented as messages sent to pm, vfs, or vm servers
3. **Testing**: Use ATF (Automated Test Framework) for new tests
4. **Build dependencies**: Always build tools first, then libraries, then applications
5. **Portability**: Tools in tools/ must be highly portable (C89 compatible where possible)
