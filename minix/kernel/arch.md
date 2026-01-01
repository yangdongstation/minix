# MINIX Microkernel Architecture

This directory contains the MINIX microkernel implementation - the minimal kernel that provides essential services while delegating most OS functionality to userspace servers.

## Core Kernel Files

### Main Kernel Components
- **main.c**: Kernel entry point and initialization
  - Sets up the microkernel environment
  - Initializes message passing system
  - Starts essential system servers

- **proc.c/proc.h**: Process management
  - Process creation, termination, and scheduling
  - Process table management
  - Context switching implementation

- **system.c/system.h**: System call interface
  - Kernel system call handlers
  - Interface between userspace and kernel
  - Privilege and permission checking

- **ipc.h/ipc_filter.h**: Inter-process communication
  - Message passing implementation
  - IPC filtering and security
  - Synchronous/asynchronous messaging

### Hardware Abstraction
- **interrupt.c/interrupt.h**: Interrupt handling
  - Hardware interrupt management
  - Interrupt routing to appropriate handlers

- **clock.c/clock.h**: System clock and timers
  - Timer interrupt handling
  - System timekeeping
  - Process scheduling timers

### Architecture Support
- **arch/**: Architecture-specific code
  - **i386/**: Intel 32-bit x86 support
  - **earm/**: ARM embedded architecture support
  - **riscv64/**: RISC-V 64-bit support (active development)

### Kernel Services
- **system/**: System service implementations
  - Memory management services
  - Process control services
  - I/O port access services
  - Interrupt management services

### Debugging and Profiling
- **debug.c/debug.h**: Kernel debugging facilities
  - Debug output and logging
  - Kernel state inspection
  - Assertion and panic handling

- **profile.c/profile.h**: Performance profiling
  - Kernel profiling support
  - Performance monitoring

### Synchronization
- **smp.c/smp.h**: Symmetric multiprocessing
  - Multi-CPU support
  - CPU-local data management

- **cpulocals.c/cpulocals.h**: Per-CPU data
  - CPU-local storage implementation
  - Thread-local data management

- **spinlock.h**: Spinlock primitives
  - Low-level synchronization primitives

### Memory Management
- **vm.h**: Virtual memory interface
  - Kernel memory management
  - Virtual memory subsystem interface

- **table.c**: Kernel data tables
  - System tables and constants

### Utility Functions
- **utility.c**: General kernel utilities
  - String operations
  - Data structure utilities

- **proto.h**: Function prototypes
  - All kernel function declarations

### Configuration
- **config.h**: Kernel configuration
  - Compile-time configuration options
  - Feature toggles

- **const.h**: Kernel constants
  - System-wide constants and limits

- **type.h**: Kernel type definitions
  - Custom data types used by kernel

- **priv.h**: Privilege definitions
  - Process privilege levels and permissions

### Watchdog and Monitoring
- **watchdog.c/watchdog.h**: System watchdog
  - System health monitoring
  - Automatic recovery mechanisms

### Build Support
- **Makefile**: Kernel build configuration
- **extract-*.sh**: Scripts to extract kernel information
- **procoffsets.h.tmp**: Process structure offsets (generated)

## RISC-V 64-bit Architecture Support

The **arch/riscv64/** directory contains RISC-V 64-bit specific implementations:

### Boot and Initialization
- **head.S**: Kernel entry point and early boot
  - Sets up initial stack and environment
  - Configures Sv39 virtual memory
  - Transitions to supervisor mode

- **kernel.lds**: Linker script for kernel layout

### Exception and Interrupt Handling
- **exception.c**: Trap and exception handling
  - Processes hardware exceptions
  - Handles system calls
  - Manages interrupt routing

- **exc.S**: Assembly exception handlers
- **hw_intr.c**: Hardware interrupt management

### Memory Management
- **memory.c**: RISC-V memory management
- **pg_utils.c**: Page table utilities
- **protect.c**: Memory protection implementation

### Hardware Interface
- **plic.c**: Platform-Level Interrupt Controller
  - External interrupt handling
  - Interrupt priority management

- **sbi.c**: Supervisor Binary Interface
  - Firmware interface for M-mode operations
  - Timer and IPI management

### Architecture Services
- **arch_system.c**: Architecture-specific system calls
- **arch_clock.c**: Clock and timer implementation
- **arch_reset.c**: System reset functionality
- **arch_do_vmctl.c**: Virtual memory control

### Low-level Functions
- **klib.S**: Kernel library functions
- **mpx.S**: Multiplexing support
- **phys_copy.S**: Physical memory copy
- **phys_memset.S**: Physical memory set

### Console and I/O
- **console.c**: Console output
- **direct_tty_utils.c**: TTY utilities

### Build Configuration
- **Makefile.inc**: RISC-V specific build rules
- **procoffsets.cf**: Process structure configuration

## Kernel Design Philosophy

The MINIX microkernel follows these principles:
1. **Minimal Kernel**: Keep the kernel as small as possible
2. **Userspace Servers**: Move OS services to userspace
3. **Message Passing**: Use IPC instead of system calls
4. **Fault Isolation**: Servers can crash without killing the system
5. **Modular Design**: Clean separation of concerns

The kernel provides only essential services:
- Process management and scheduling
- Inter-process communication
- Low-level hardware abstraction
- Memory management primitives
- Basic I/O operations

All other functionality is implemented in userspace servers that communicate with the kernel and each other through a well-defined message-passing interface.