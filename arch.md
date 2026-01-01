# MINIX 3 Root Directory Architecture

This is the root directory of the MINIX 3 operating system source code repository. MINIX 3 is a microkernel-based operating system designed for high reliability and fault tolerance.

## 本仓库实际包含/可选或上游组件

本仓库实际包含：以下为顶层条目示例（完整清单以目录实际内容为准）：`minix/`, `sys/`, `include/`, `lib/`, `tools/`, `distrib/`, `releasetools/`, `etc/`, `docs/`, `share/`, `tests/`, `build.sh`, `Makefile`.
可选或上游组件：上文条目可能包含上游/可选组件示例，当前仓库未必包含全部。

## Directory Structure

### Core MINIX Components
- **minix/**: Core MINIX-specific source code including kernel, servers, drivers, and utilities
- **sys/**: System headers and kernel interface definitions
- **include/**: Standard C library headers and system includes
- **lib/**: Standard C library and support libraries

### NetBSD Compatibility Layer
- **bin/**: Essential user utilities (cat, ls, cp, etc.) synced with NetBSD-8
- **sbin/**: System administration utilities (ifconfig, route, etc.)
- **usr.bin/**: Additional user utilities (awk, sed, vi, etc.)
- **usr.sbin/**: Additional system administration utilities

### Build Infrastructure
- **tools/**: Cross-compilation toolchain and build utilities
- **external/**: Third-party software organized by license
- **distrib/**: Distribution building scripts and configuration
- **releasetools/**: Release packaging and distribution tools

### Configuration and Documentation
- **etc/**: System configuration files and service definitions
- **docs/**: Documentation and manuals
- **share/**: Architecture-independent data files
- **games/**: Games and entertainment programs

### Development Support
- **common/**: Common build definitions and shared files
- **gnu/**: GNU utilities and libraries
- **crypto/**: Cryptographic libraries and utilities
- **libexec/**: Programs executed by other programs
- **tests/**: Test suites for various components

### Key Build Files
- **build.sh**: Main build script for cross-compilation
- **Makefile**: Top-level makefile
- **Makefile.inc**: Common makefile includes
- **LICENSE**: Project license information
- **AGENTS.md**: Development guide for AI agents

### Architecture-Specific Files
- **minix-riscv64.img**: Pre-built RISC-V 64-bit disk image
- **build-*.log**: Build logs for different configurations
- **README-RISCV64.md**: RISC-V specific documentation
- **RISC64-STATUS.md**: RISC-V port development status

## Project Focus

The project is actively developing RISC-V 64-bit support for the QEMU virt platform, with approximately 65% completion of the port. The microkernel architecture provides fault tolerance through userspace servers that can crash and restart without affecting system stability.
