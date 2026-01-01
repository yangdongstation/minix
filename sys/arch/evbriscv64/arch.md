# RISC-V 64-bit System Headers Architecture

## Overview

The `sys/arch/evbriscv64/` directory contains architecture-specific system headers and definitions for the RISC-V 64-bit port of MINIX 3. This directory provides the essential header files that define RISC-V-specific constants, data structures, and interfaces required for system-level programming on RISC-V platforms.

## Purpose and Responsibilities

### Core Functions
- **Architecture Definitions**: Provides RISC-V-specific constants and macros
- **Hardware Interface**: Defines hardware register layouts and bit definitions
- **System Call Interface**: Specifies architecture-specific system call conventions
- **Memory Layout**: Defines memory maps and address spaces for RISC-V
- **Platform Configuration**: Contains platform-specific configuration parameters
- **Binary Interface**: Defines Application Binary Interface (ABI) conventions

### Key Features
- RISC-V 64-bit architecture support (RV64GC)
- Platform-specific hardware definitions
- Supervisor Binary Interface (SBI) integration
- Memory management unit (MMU) support
- Interrupt and exception handling definitions
- Performance monitoring interfaces

## Directory Structure

```
sys/arch/evbriscv64/
├── include/            # Architecture-specific header files
│   └── Makefile       # Build configuration for headers
└── Makefile           # Main architecture makefile
```

## Header File Organization

### System Architecture Headers
The directory structure follows MINIX conventions for architecture-specific headers:

```
sys/arch/evbriscv64/include/
├── arch/              # Architecture-specific definitions
│   ├── atomic.h       # Atomic operation definitions
│   ├── bootinfo.h     # Boot information structures
│   ├── bus.h          # Bus interface definitions
│   ├── cpu.h          # CPU-specific definitions
│   ├── endian.h       # Endianness definitions
│   ├── endian_machdep.h # Machine-dependent endian definitions
│   ├── frame.h        # Stack frame definitions
│   ├── intr.h         # Interrupt handling definitions
│   ├── lock.h         # Lock primitive definitions
│   ├── mutex.h        # Mutex implementation
│   ├── param.h        # Architecture parameters
│   ├── pcb.h          # Process Control Block definitions
│   ├── pmap.h         # Physical map definitions
│   ├── proc.h         # Process structure definitions
│   ├── profile.h      # Profiling support
│   ├── ptrace.h       # Process tracing definitions
│   ├── reg.h          # Register definitions
│   ├── reloc.h        # Relocation definitions
│   ├── setjmp.h       # Non-local jump definitions
│   ├── signal.h       # Signal handling definitions
│   ├── stack.h        # Stack definitions
│   ├── trap.h         # Trap/exception definitions
│   ├── types.h        # Architecture types
│   ├── varargs.h      # Variable argument handling
│   └── vmparam.h       # Virtual memory parameters
```

## RISC-V 64-bit Specific Definitions

### Architecture Parameters (`param.h`)
```c
/* RISC-V 64-bit architecture parameters */
#define MACHINE_ARCH        "riscv64"
#define MACHINE             "evbriscv64"
#define PAGE_SHIFT          12              /* 4KB pages */
#define PAGE_SIZE           (1UL << PAGE_SHIFT)
#define PAGE_MASK           (PAGE_SIZE - 1)
#define NBPG                PAGE_SIZE       /* Bytes per page */
#define PGSHIFT             PAGE_SHIFT      /* LOG2(PAGE_SIZE) */

/* RISC-V specific parameters */
#define RISCV64_PAGE_SHIFT  12
#define RISCV64_MAXADDR     (1UL << 56)     /* 56-bit virtual address space */
#define RISCV64_MAXUSERADDR (1UL << 38)     /* 38-bit user address space */
```

### Register Definitions (`reg.h`)
RISC-V register layout and conventions:
```c
/* RISC-V general purpose registers */
struct reg {
    unsigned long ra;       /* Return address (x1) */
    unsigned long sp;       /* Stack pointer (x2) */
    unsigned long gp;       /* Global pointer (x3) */
    unsigned long tp;       /* Thread pointer (x4) */
    unsigned long t0;       /* Temporary 0 (x5) */
    unsigned long t1;       /* Temporary 1 (x6) */
    unsigned long t2;       /* Temporary 2 (x7) */
    unsigned long s0;       /* Saved register 0 (x8) */
    unsigned long s1;       /* Saved register 1 (x9) */
    /* ... additional registers ... */
    unsigned long pc;       /* Program counter */
};

/* RISC-V control and status registers */
struct csr_reg {
    unsigned long status;   /* Machine/Supervisor status */
    unsigned long epc;      /* Exception program counter */
    unsigned long cause;    /* Exception cause */
    unsigned long tval;     /* Trap value */
    unsigned long cycle;    /* Cycle counter */
    unsigned long time;     /* Time counter */
    unsigned long instret;  /* Instruction counter */
};
```

### Virtual Memory Parameters (`vmparam.h`)
RISC-V memory management definitions:
```c
/* RISC-V Sv39 virtual memory parameters */
#define RISCV64_VM_SV39_LEVELS      3       /* 3-level page table */
#define RISCV64_VM_SV39_VPN_SHIFT   9       /* VPN field width */
#define RISCV64_VM_SV39_PAGE_SHIFT  12      /* Page offset bits */
#define RISCV64_VM_SV39_ASID_BITS   16      /* ASID width */

/* Virtual address space layout */
#define USER_MIN_ADDRESS    0x0000000000000000UL
#define USER_MAX_ADDRESS    0x0000003FFFFFFFFFUL  /* 38-bit user space */
#define KERNEL_MIN_ADDRESS  0xFFFFFFFFC0000000UL  /* Upper 1GB */
#define KERNEL_MAX_ADDRESS  0xFFFFFFFFFFFFFFFFUL
```

### Trap Definitions (`trap.h`)
RISC-V exception and interrupt handling:
```c
/* RISC-V exception codes */
#define RISCV_EXCEPTION_INST_MISALIGNED     0
#define RISCV_EXCEPTION_INST_ACCESS_FAULT   1
#define RISCV_EXCEPTION_ILLEGAL_INST        2
#define RISCV_EXCEPTION_BREAKPOINT          3
#define RISCV_EXCEPTION_LOAD_MISALIGNED     4
#define RISCV_EXCEPTION_LOAD_ACCESS_FAULT   5
#define RISCV_EXCEPTION_STORE_MISALIGNED    6
#define RISCV_EXCEPTION_STORE_ACCESS_FAULT  7
#define RISCV_EXCEPTION_ENV_CALL_U          8
#define RISCV_EXCEPTION_ENV_CALL_S          9
#define RISCV_EXCEPTION_ENV_CALL_M          11
#define RISCV_EXCEPTION_INST_PAGE_FAULT     12
#define RISCV_EXCEPTION_LOAD_PAGE_FAULT     13
#define RISCV_EXCEPTION_STORE_PAGE_FAULT    15

/* RISC-V interrupt codes */
#define RISCV_INTERRUPT_SOFT                1
#define RISCV_INTERRUPT_TIMER               5
#define RISCV_INTERRUPT_EXTERNAL            9
```

## Platform Integration

### QEMU virt Platform Support
QEMU virt machine specific definitions:
```c
/* QEMU virt machine memory map */
#define VIRT_CLINT_ADDR     0x02000000UL    /* Core Local Interruptor */
#define VIRT_CLINT_SIZE     0x00010000UL
#define VIRT_PLIC_ADDR      0x0C000000UL    /* Platform-Level Interrupt Controller */
#define VIRT_PLIC_SIZE      0x04000000UL
#define VIRT_UART0_ADDR     0x10000000UL    /* NS16550 UART */
#define VIRT_UART0_SIZE     0x00000100UL
#define VIRT_VIRTIO_ADDR    0x10001000UL    /* VirtIO devices */
```

### SBI Integration
Supervisor Binary Interface definitions:
```c
/* SBI extension IDs */
#define SBI_EXT_BASE                0x10
#define SBI_EXT_TIME                0x54494D45
#define SBI_EXT_IPI                 0x735049
#define SBI_EXT_RFENCE              0x52464E43
#define SBI_EXT_HSM                 0x48534D

/* SBI function IDs */
#define SBI_BASE_GET_SPEC_VERSION   0
#define SBI_BASE_GET_IMP_ID         1
#define SBI_BASE_GET_IMP_VERSION    2
#define SBI_BASE_PROBE_EXT          3
#define SBI_BASE_GET_MVENDORID      4
#define SBI_BASE_GET_MARCHID        5
#define SBI_BASE_GET_MIMPID         6
```

## Build System Integration

### Makefile Configuration
```makefile
# RISC-V 64-bit architecture makefile
ARCHDIR = ${.CURDIR}
.include "${.CURDIR}/../Makefile.inc"

# Architecture-specific definitions
ARCH = riscv64
MACHINE_ARCH = riscv64
MACHINE = evbriscv64
```

### Header Installation
The build system automatically installs architecture headers to the appropriate system directories, making them available for kernel and system programming.

## ABI Conventions

### Calling Convention
RISC-V 64-bit calling convention support:
- Integer arguments in registers a0-a7
- Floating-point arguments in fa0-fa7
- Stack alignment requirements
- Return value conventions
- Register preservation rules

### Stack Layout
RISC-V stack frame definitions:
```c
/* Stack frame layout */
struct trapframe {
    unsigned long ra;       /* Return address */
    unsigned long sp;       /* Stack pointer */
    unsigned long gp;       /* Global pointer */
    /* ... saved registers ... */
    unsigned long pc;       /* Program counter */
    unsigned long status;   /* Status register */
};
```

## Memory Model

### Weak Ordering
RISC-V weak memory model support:
- Memory barrier definitions
- Atomic operation primitives
- Cache coherence considerations
- Memory ordering constraints

### Atomic Operations
Architecture-specific atomic primitives:
```c
/* Atomic operation definitions */
#define atomic_set_int(p, v)    __atomic_store_n((p), (v), __ATOMIC_SEQ_CST)
#define atomic_add_int(p, v)    __atomic_add_fetch((p), (v), __ATOMIC_SEQ_CST)
#define atomic_cmpset_int(p, o, n) __atomic_compare_exchange_n((p), &(o), (n), 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
```

## Integration with System Headers

### System-Wide Availability
The architecture headers are integrated into the main MINIX header system:
- Automatic inclusion in system builds
- Cross-architecture compatibility
- Conditional compilation support
- Platform-specific optimizations

### Kernel Integration
Headers provide essential definitions for:
- Kernel memory management
- Process scheduling
- Interrupt handling
- System call interfaces
- Device driver development

## Testing and Validation

### Header Validation
Architecture header validation includes:
- Compile-time verification
- Cross-reference checking
- ABI compliance validation
- Platform-specific testing
- Integration testing

### Compatibility Testing
Ensures compatibility with:
- RISC-V specifications
- MINIX system requirements
- Third-party software expectations
- Toolchain requirements
- Platform conventions

## Future Enhancements

### RISC-V Evolution
Support for future RISC-V developments:
- New ISA extensions
- Enhanced platform features
- Advanced interrupt handling
- Improved performance monitoring
- Security extensions

### Platform Expansion
Support for additional RISC-V platforms:
- SiFive platform headers
- Embedded system optimizations
- Real-time extensions
- Virtualization support
- Custom platform definitions

The RISC-V 64-bit system headers provide the essential architectural definitions that enable MINIX 3 to operate correctly on RISC-V platforms while maintaining compatibility with the broader MINIX system architecture and supporting the unique features of the RISC-V instruction set architecture.