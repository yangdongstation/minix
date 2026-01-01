# MINIX Core Directory Architecture

This directory contains the core MINIX-specific source code that implements the microkernel-based operating system. Unlike the NetBSD-compatible directories in the root, these components are unique to MINIX 3's microkernel architecture.

## 本仓库实际包含/可选或上游组件

本仓库实际包含：以下为顶层条目示例（完整清单以目录实际内容为准）：`kernel/`, `servers/`, `drivers/`, `fs/`, `lib/`, `net/`, `tests/`.
可选或上游组件：上文条目可能包含上游/可选组件示例，当前仓库未必包含全部。

## Directory Structure

### Core System Components
- **kernel/**: The microkernel implementation with architecture-specific code
  - Contains the minimal kernel that handles message passing, process management, and hardware abstraction
  - Architecture support for i386, ARM (earm), and RISC-V 64-bit (evbriscv64)

- **servers/**: Userspace system servers that implement OS functionality
  - **pm/**: Process manager - handles process creation, termination, and signals
  - **vfs/**: Virtual file system server - manages file system operations
  - **vm/**: Virtual memory manager - handles memory allocation and paging
  - **rs/**: Reincarnation server - monitors and restarts crashed servers
  - **devman/**: Device manager - coordinates hardware device access

### Device Drivers
- **drivers/**: Hardware drivers running as userspace processes
  - **tty/**: Terminal drivers including NS16550 UART for RISC-V
  - **memory/**: Memory device drivers
  - **example/**: Template for new driver development

### File Systems
- **fs/**: File system implementations
  - **mfs/**: MINIX file system implementation
  - **pipefs/**: Pipe file system for inter-process communication

### Libraries and Utilities
- **lib/**: MINIX-specific libraries
  - **syslib/**: System call interface library
  - **timers/**: Timer management library
  - **liveupdate/**: Live update functionality

- **commands/**: MINIX-specific user commands
  - **service/**: Service management commands
  - **update/**: System update utilities

### Configuration and Documentation
- **etc/**: MINIX-specific configuration files
- **man/**: Manual pages for MINIX commands
- **share/**: Shared MINIX-specific data files

### Network Support
- **net/**: Network stack and protocols
  - **inet/**: TCP/IP protocol implementation
  - **drivers/**: Network device drivers

### Testing and Development
- **tests/**: Comprehensive test suite
  - **riscv64/**: RISC-V 64-bit specific tests
  - **kernel/**: Kernel functionality tests
  - **user/**: Userspace tests

- **scripts/**: Development and debugging scripts
  - QEMU launch scripts for different architectures
  - GDB debugging scripts

### Build Support
- **sbin/**: MINIX-specific system binaries
- **usr.bin/**: Additional MINIX user utilities
- **usr.sbin/**: Additional MINIX system utilities
- **llvm/**: LLVM-related build support
- **releasetools/**: Release-specific tools

### Makefiles
- **Makefile**: Top-level MINIX makefile
- **Makefile.inc**: Common MINIX build definitions
- **Makefile.fetchgnu**: Script to fetch GNU components

## Architecture Philosophy

This directory implements MINIX 3's microkernel philosophy where:
1. The kernel is minimal and only handles essential functions
2. Most OS services run as userspace servers
3. Communication happens through message passing, not system calls
4. Servers can crash and be restarted without affecting system stability
5. Each component has minimal privileges and specific IPC permissions

The RISC-V 64-bit port development is primarily focused in the kernel/arch/riscv64/ directory, with ongoing work to bring full functionality to this architecture.
