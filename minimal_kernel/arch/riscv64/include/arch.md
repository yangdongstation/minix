# MINIX RISC-V 64-bit Architecture Headers

This directory contains the architecture-specific header files for RISC-V 64-bit support in the MINIX kernel. These headers define the interface between the generic kernel code and the RISC-V hardware abstraction layer.

## Core Architecture Definitions

### Hardware Constants
- **archconst.h**: Fundamental RISC-V hardware constants
  - Register definitions and offsets
  - Memory layout constants
  - Page size definitions (4KB)
  - Cache line sizes
  - Timer frequency values
  - Interrupt vector addresses
  - Privilege level definitions
  - CSR (Control and Status Register) addresses
  - Physical memory boundaries
  - Device memory maps

### Function Prototypes
- **arch_proto.h**: Architecture function declarations
  - Clock and timer function prototypes
  - Memory management interfaces
  - Interrupt handling functions
  - Exception processing routines
  - Context switching functions
  - Hardware initialization routines
  - Platform-specific operations
  - Low-level utility functions
  - Assembly language interfaces
  - Device driver support functions

### Hardware Interrupt Interface
- **hw_intr.h**: Hardware interrupt definitions
  - IRQ number assignments
  - Interrupt priority levels
  - Interrupt mask definitions
  - PLIC (Platform-Level Interrupt Controller) interface
  - CLINT (Core Local Interruptor) definitions
  - Timer interrupt configuration
  - Software interrupt handling
  - External interrupt routing
  - Interrupt handler registration

## Clock and Timer Support

### Clock Interface
- **arch_clock.h**: Clock and timer definitions
  - Timer register definitions
  - Clock frequency constants
  - Timer interrupt vectors
  - Timebase frequency
  - Timer comparison values
  - Tick rate definitions
  - Clock calibration constants
  - Timer mode settings
  - Real-time clock interface

### System Constants
- **sconst.h**: System-wide constants
  - Assembly-time constants
  - Processor-specific values
  - Memory alignment requirements
  - Stack alignment constraints
  - Register save area sizes
  - Context switch frame layouts
  - Exception stack requirements
  - Privilege stack definitions

## Architecture Interface Categories

### Memory Management
- Page table entry formats
  - Sv39 page table structures
  - Page protection bits
  - Address translation flags
  - Memory type definitions
  - Cache policy settings

### Interrupt Architecture
- Interrupt vector table layout
  - Exception code assignments
  - Interrupt priority schemes
  - Vector spacing requirements
  - Handler entry conventions

### Context Management
- Process context structures
  - Register save layouts
  - Floating-point context
  - Vector register support
  - Special register handling

### Platform Integration
- Platform capability flags
  - Feature detection masks
  - Hardware version information
  - Platform-specific constants
  - Device tree integration

## RISC-V 64-bit Specific Features

### Register Definitions
- General-purpose registers (x0-x31)
- Control and status registers
- Performance monitoring counters
- Debug register definitions
- Floating-point registers
- Vector register support

### Privilege Levels
- Machine mode (M-mode) definitions
- Supervisor mode (S-mode) interfaces
- User mode (U-mode) constraints
- Mode transition mechanisms
- Privilege escalation prevention

### Memory Model
- Sv39 virtual memory specifications
- 3-level page table structures
- 512GB address space per process
- Large page support (2MB, 1GB)
- Memory ordering semantics
- Cache coherence protocols

### Atomic Operations
- Load-reserved/store-conditional
- Atomic memory operations (AMO)
- Memory barrier instructions
- Ordering constraints
- Multi-core synchronization

## Integration with MINIX

### Kernel Interface
All headers integrate with MINIX's microkernel design:
- Message-passing compatibility
- Type-safe interfaces
- Error code definitions
- Permission validation
- Security boundaries

### Hardware Abstraction
Clean abstraction between hardware and kernel:
- Platform-independent interfaces
- Hardware feature detection
- Capability reporting
- Dynamic configuration support

### Performance Considerations
- Inline function definitions
- Assembly macro support
- Optimized constants
- Cache-friendly layouts
- Alignment optimizations

## Usage Guidelines

### Header Inclusion
- Include order dependencies
- Cross-header references
- Circular dependency prevention
- Feature test macros
- Conditional compilation

### Portability
- Architecture-neutral interfaces
- Multi-platform support
- Feature detection mechanisms
- Graceful degradation
- Backward compatibility

### Development Support
- Debug interface definitions
- Profiling support
- Tracing mechanisms
- Diagnostic interfaces
- Testing hooks

These architecture headers provide the complete interface definition for RISC-V 64-bit hardware support in MINIX, ensuring type safety, performance optimization, and clean integration with the microkernel architecture while maintaining portability and extensibility for future RISC-V implementations.