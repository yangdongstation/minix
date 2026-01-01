# MINIX RISC-V 64-bit Kernel Architecture

This directory contains the complete RISC-V 64-bit architecture support for the MINIX microkernel, implementing all hardware-specific functionality required for the RISC-V RV64GC instruction set architecture.

## Core Architecture Files

### Boot and Initialization
- **head.S**: Kernel entry point and early boot sequence
  - Initial processor setup
  - Stack initialization
  - Virtual memory configuration (Sv39)
  - Transition from M-mode to S-mode
  - Exception vector setup
  - Initial memory mapping

- **kernel.lds**: Linker script for kernel layout
  - Memory section organization
  - Symbol placement
  - Alignment requirements
  - Special section definitions

- **pre_init.c**: Pre-initialization code
  - Very early C environment setup
  - Basic hardware detection
  - Minimal console output
  - Pre-main initialization

### Exception and Interrupt Handling
- **exception.c**: Primary exception handler
  - Trap frame processing
  - Exception classification
  - Hardware exception handling
  - System call dispatch
  - Fault recovery
  - User/kernel mode transitions

- **exc.S**: Assembly exception entry points
  - Low-level trap handling
  - Register preservation
  - Stack switching
  - Exception vector code
  - Fast path optimizations

### Hardware Abstraction
- **sbi.c**: Supervisor Binary Interface
  - M-mode firmware interface
  - Timer management via SBI
  - Inter-processor interrupts
  - Console I/O through SBI
  - System reset and shutdown
  - Performance monitoring

- **plic.c**: Platform-Level Interrupt Controller
  - External interrupt management
  - IRQ priority handling
  - Interrupt distribution
  - Multi-core interrupt routing
  - Device interrupt configuration
  - Context management

- **hw_intr.c**: Hardware interrupt handling
  - Interrupt vector management
  - IRQ registration
  - Interrupt masking
  - Priority handling
  - Nested interrupt support

### Memory Management
- **memory.c**: Physical memory management
  - Memory detection
  - Physical memory mapping
  - Memory region management
  - DMA memory allocation
  - Cache management
  - Memory topology

- **pg_utils.c**: Page table utilities
  - Sv39 page table management
  - Virtual address translation
  - Page fault handling
  - TLB management
  - Memory protection
  - Address space manipulation

- **protect.c**: Memory protection
  - Access permission management
  - Memory region protection
  - Privilege level enforcement
  - Security policy implementation
  - Memory isolation

### Architecture-Specific System Calls
- **arch_system.c**: Architecture system calls
  - RISC-V specific system services
  - Hardware feature detection
  - CPU capability reporting
  - Architecture-specific operations

- **arch_clock.c**: Clock and timer support
  - Timer initialization
  - Clock source management
  - Timer interrupt handling
  - Timekeeping
  - Scheduler tick generation

- **arch_do_vmctl.c**: Virtual memory control
  - Architecture-specific VM operations
  - Page table manipulation
  - Memory mapping control
  - Cache operations

- **arch_reset.c**: System reset
  - Hardware reset sequences
  - System restart
  - Shutdown procedures
  - Clean system termination

### Low-Level Utilities
- **klib.S**: Kernel library functions
  - Optimized memory operations
  - String manipulation
  - Bit operations
  - Arithmetic functions
  - Architecture primitives

- **mpx.S**: Multiplexing support
  - Context switching
  - Register save/restore
  - Multi-processing support
  - CPU state management

- **phys_copy.S**: Physical memory copying
  - Optimized memory copy
  - Cache-aware operations
  - Alignment handling
  - Performance optimization

- **phys_memset.S**: Physical memory setting
  - Bulk memory initialization
  - Cache-efficient operations
  - Aligned memory fills
  - Performance-critical paths

### Console and I/O
- **console.c**: Console output
  - Early console initialization
  - Debug output support
  - Serial console management
  - Emergency output
  - Kernel message display

- **direct_tty_utils.c**: Direct TTY utilities
  - Low-level terminal support
  - Character output
  - Console control
  - Debug interface

### Configuration and Build
- **procoffsets.cf**: Process structure configuration
  - Offset definitions
  - Structure layout
  - Assembly constants
  - Build-time configuration

- **Makefile.inc**: Build configuration
  - Compilation flags
  - Architecture settings
  - Optimization options
  - Debug configuration

- **README.md**: Architecture documentation
  - Implementation notes
  - Architecture details
  - Development guidelines
  - Platform-specific information

### Board Support Package
- **bsp/**: Board Support Package
  - **virt/**: QEMU virt platform support
    - Platform detection
    - Device tree parsing
    - Memory map configuration
    - Platform initialization
    - Device configuration

### Include Files
- **include/**: Architecture headers
  - Register definitions
  - Hardware constants
  - Inline functions
  - Assembly macros

## Key Features

### RISC-V 64-bit Support
- RV64GC instruction set
- 64-bit register operations
- Large address space support
- Atomic instruction support
- Compressed instruction support

### Virtual Memory
- Sv39 virtual memory model
- 3-level page tables
- 512GB address space per process
- Demand paging support
- Copy-on-write implementation

### Interrupt Architecture
- CLINT for timer and software interrupts
- PLIC for external interrupts
- Vectorized interrupt handling
- Priority-based interrupt routing
- Nested interrupt support

### Multi-Core Support
- SMP initialization
- Inter-processor interrupts
- CPU-local storage
- Spinlock implementation
- Memory barriers

## Integration with MINIX

### Message Passing
All architecture functions integrate with MINIX's message-passing paradigm:
- Structured parameter passing
- Type-safe interfaces
- Error code propagation
- Synchronous communication

### Security Model
Hardware-enforced security:
- Privilege level separation
- Memory protection
- Access control validation
- Secure boot support

### Performance Optimizations
- Assembly language optimizations
- Cache-aware algorithms
- TLB efficiency
- Branch prediction hints
- Memory alignment

This RISC-V 64-bit implementation provides a complete, efficient, and secure hardware abstraction layer for the MINIX microkernel, enabling full system functionality on RISC-V platforms while maintaining compatibility with MINIX's design principles.