# Intel x86 Architecture System Headers

## Overview

The `sys/arch/i386/` directory contains architecture-specific system headers and definitions for Intel x86 (32-bit) processors in MINIX 3. This directory provides the essential header files that define x86-specific constants, data structures, and interfaces required for system-level programming on traditional PC-compatible hardware platforms.

## Purpose and Responsibilities

### Core Functions
- **x86 Architecture Support**: Provides comprehensive Intel x86 (32-bit) processor support
- **PC Platform Integration**: Defines interfaces for traditional PC hardware components
- **Legacy Compatibility**: Maintains compatibility with existing x86-based MINIX installations
- **Boot Architecture**: Supports traditional BIOS and modern UEFI boot mechanisms
- **Device Interface**: Defines interfaces for x86-specific hardware devices

### Key Features
- **Segmented Memory Model** - x86 segment register support
- **Protected Mode Operation** - Full 32-bit protected mode implementation
- **Interrupt Descriptor Table** - IDT management and interrupt handling
- **I/O Port Access** - Direct hardware I/O port operations
- **MultiProcessor Support** - SMP (Symmetric MultiProcessing) capabilities

## Directory Structure

```
sys/arch/i386/
├── conf/                    # Architecture-specific configuration
│   ├── Makefile.evbi386    # Evaluation board i386 configuration
│   ├── GENERIC            # Generic i386 kernel configuration
│   ├── GENERIC.local      # Local modifications
│   ├── GENERIC_MP         # Multi-processor configuration
│   └── GENERIC_MP.local   # MP local modifications
├── include/                 # Architecture-specific header files
│   ├── Makefile           # Header build configuration
│   ├── arch/              # Core architecture definitions
│   │   ├── atomic.h       # Atomic operation definitions
│   │   ├── biosvar.h      # BIOS interface variables
│   │   ├── bootinfo.h     # Boot information structures
│   │   ├── bus.h          # Bus interface definitions
│   │   ├── cdefs.h        # C compiler definitions
│   │   ├── conf.h         # Configuration definitions
│   │   ├── cpu.h          # CPU-specific definitions
│   │   ├── endian.h       # Endianness definitions
│   │   ├── endian_machdep.h # Machine-dependent endian
│   │   ├── frame.h        # Stack frame definitions
│   │   ├── ieeefp.h       # IEEE floating point
│   │   ├── inline.h       # Inline assembly definitions
│   │   ├── intr.h         # Interrupt handling definitions
│   │   ├── lock.h         # Lock primitive definitions
│   │   ├── mutex.h        # Mutex implementation
│   │   ├── npx.h          # Numeric processor extension
│   │   ├── param.h        # Architecture parameters
│   │   ├── pcb.h          # Process Control Block
│   │   ├── pmap.h         # Physical map definitions
│   │   ├── proc.h         # Process structure definitions
│   │   ├── profile.h      # Profiling support
│   │   ├── ptrace.h       # Process tracing definitions
│   │   ├── reg.h          # Register definitions
│   │   ├── reloc.h        # Relocation definitions
│   │   ├── setjmp.h       # Non-local jump definitions
│   │   ├── signal.h       # Signal handling definitions
│   │   ├── specialreg.h   # Special register definitions
│   │   ├── stack.h        # Stack definitions
│   │   ├── trap.h         # Trap/exception definitions
│   │   ├── types.h        # Architecture types
│   │   ├── varargs.h      # Variable argument handling
│   │   └── vmparam.h      # Virtual memory parameters
│   ├── bootblock.h        # Boot block definitions
│   ├── bootinfo.h         # Boot information header
│   ├── bus_defs.h         # Bus definition constants
│   ├── cputypes.h         # CPU type definitions
│   ├── db_machdep.h       # Machine-dependent debugger
│   ├── dentry.h           # Directory entry structures
│   ├── disklabel.h        # Disk label definitions
│   ├── elf_machdep.h      # ELF format dependencies
│   ├── endian.h           # Endianness definitions
│   ├── endian_machdep.h   # Machine-dependent endian
│   ├── float.h            # Floating point definitions
│   ├── frame.h            # Stack frame header
│   ├── ieee.h             # IEEE floating point
│   ├── ieee754.h          # IEEE 754 floating point
│   ├── ieeefp.h           # IEEE FP definitions
│   ├── limits.h           # System limits
│   ├── locking.h          # Locking definitions
│   ├── mcontext.h         # Machine context definitions
│   ├── memory.h           # Memory management definitions
│   ├── mutex.h            # Mutex definitions
│   ├── param.h            # System parameters
│   ├── pcb.h              # Process control block header
│   ├── pmap.h             # Physical map header
│   ├── proc.h             # Process definitions header
│   ├── profile.h          # Profiling header
│   ├── pthread.h          # POSIX thread definitions
│   ├── ptrace.h           # Process tracing header
│   ├── reg.h              # Register header
│   ├── segments.h         # Segment definitions
│   ├── setjmp.h           # Non-local jumps header
│   ├── signal.h           # Signal handling header
│   ├── specialreg.h       # Special registers header
│   ├── stack.h            # Stack header
│   ├── stdarg.h           # Standard argument handling
│   ├── trap.h             # Trap definitions header
│   ├── types.h            # Type definitions header
│   ├── varargs.h          # Variable arguments header
│   └── vmparam.h          # Virtual memory parameters header
├── Makefile               # Main architecture makefile
└── stand/                 # Standalone boot code
    ├── Makefile           # Standalone build configuration
    ├── bootinfo.h         # Boot information for standalone
    ├── lib.h              # Standalone library definitions
    ├── pxe.h              # PXE boot definitions
    └── seg.h              # Segment definitions for boot
```

## x86 Architecture-Specific Definitions

### Architecture Parameters (`include/arch/param.h`)
```c
/* Intel x86 architecture parameters */
#define MACHINE_ARCH        "i386"
#define MACHINE             "i386"
#define PAGE_SHIFT          12              /* 4KB pages */
#define PAGE_SIZE           (1UL << PAGE_SHIFT)
#define PAGE_MASK           (PAGE_SIZE - 1)
#define NBPG                PAGE_SIZE       /* Bytes per page */
#define PGSHIFT             PAGE_SHIFT      /* LOG2(PAGE_SIZE) */

/* x86-specific parameters */
#define I386_PAGE_SHIFT     12
#define I386_MAXADDR        0xFFFFFFFF      /* 4GB address space */
#define I386_MAXUSERADDR    0xBFFFFFFF      /* 3GB user space */

/* Segment selectors */
#define GSEL(s, r)          (((s) << 3) | r) /* Global selector */
#define LSEL(s, r)          (((s) << 3) | r | 4) /* Local selector */
```

### Register Definitions (`include/arch/reg.h`)
Intel x86 register layout and conventions:
```c
/* Intel general purpose registers */
struct reg {
    unsigned int r_eax;     /* Accumulator register */
    unsigned int r_ebx;     /* Base register */
    unsigned int r_ecx;     /* Counter register */
    unsigned int r_edx;     /* Data register */
    unsigned int r_esi;     /* Source index */
    unsigned int r_edi;     /* Destination index */
    unsigned int r_ebp;     /* Base pointer */
    unsigned int r_esp;     /* Stack pointer */
    unsigned int r_eip;     /* Instruction pointer */
    unsigned int r_eflags;  /* Flags register */
    unsigned int r_cs;      /* Code segment */
    unsigned int r_ds;      /* Data segment */
    unsigned int r_es;      /* Extra segment */
    unsigned int r_fs;      /* F segment */
    unsigned int r_gs;      /* G segment */
    unsigned int r_ss;      /* Stack segment */
};

/* x86 control registers */
struct cr_reg {
    unsigned long cr0;      /* Control register 0 */
    unsigned long cr2;      /* Control register 2 (page fault) */
    unsigned long cr3;      /* Control register 3 (page directory) */
    unsigned long cr4;      /* Control register 4 */
};
```

### Virtual Memory Parameters (`include/arch/vmparam.h`)
x86 memory management definitions:
```c
/* x86 virtual memory parameters */
#define I386_VM_MIN_ADDRESS     0x00000000
#define I386_VM_MAXUSER_ADDRESS 0xBFFFFFFF  /* 3GB user space */
#define I386_VM_MAX_ADDRESS     0xFFFFFFFF  /* 4GB total space */

/* x86 page table structures */
#define PDE_SHIFT               22          /* Page directory entry shift */
#define PTE_SHIFT               12          /* Page table entry shift */
#define NPTE                    1024        /* Entries per page table */
#define NPDE                    1024        /* Entries per page directory */

/* x86 page protection flags */
#define PG_V                    0x001       /* Page is valid */
#define PG_RW                   0x002       /* Page is writable */
#define PG_U                    0x004       /* Page is user-accessible */
#define PG_PWT                  0x008       /* Page write-through */
#define PG_PCD                  0x010       /* Page cache disable */
#define PG_A                    0x020       /* Page accessed */
#define PG_D                    0x040       /* Page dirty */
#define PG_PS                   0x080       /* Page size (4MB) */
#define PG_G                    0x100       /* Page global */
```

### Trap Definitions (`include/arch/trap.h`)
Intel x86 exception and interrupt handling:
```c
/* x86 exception codes */
#define T_DIVIDE        0       /* Division by zero */
#define T_DEBUG         1       /* Debug exception */
#define T_NMI           2       /* Non-maskable interrupt */
#define T_BRKPT         3       /* Breakpoint */
#define T_OFLOW         4       /* Overflow */
#define T_BOUND         5       /* Bound range exceeded */
#define T_ILLOP         6       /* Invalid opcode */
#define T_DEVICE        7       /* Device not available */
#define T_DBLFLT        8       /* Double fault */
#define T_COPROC        9       /* Coprocessor segment overrun */
#define T_TSS           10      /* Invalid TSS */
#define T_SEGNP         11      /* Segment not present */
#define T_STACK         12      /* Stack fault */
#define T_GPFLT         13      /* General protection fault */
#define T_PGFLT         14      /* Page fault */
#define T_RES           15      /* Reserved */
#define T_FPERR         16      /* Floating point error */
#define T_ALIGN         17      /* Alignment check */
#define T_MCHK          18      /* Machine check */
#define T_SIMDERR       19      /* SIMD floating point exception */

/* x86 interrupt vectors */
#define IRQ0            32      /* Base of IRQ0 */
#define IRQ_TIMER       0       /* Timer interrupt */
#define IRQ_KEYBOARD    1       /* Keyboard interrupt */
#define IRQ_CASCADE     2       /* Cascade interrupt */
#define IRQ_SERIAL2     3       /* Serial port 2 */
#define IRQ_SERIAL1     4       /* Serial port 1 */
#define IRQ_PARALLEL2   5       /* Parallel port 2 */
#define IRQ_FLOPPY      6       /* Floppy disk */
#define IRQ_PARALLEL1   7       /* Parallel port 1 */
```

## Platform Integration

### PC Platform Support
Traditional PC platform specific definitions:
```c
/* PC platform hardware definitions */
#define PC_IO_BASE      0x0000  /* I/O port base */
#define PC_MEM_BASE     0x00000 /* Memory base */

/* Standard PC device ports */
#define KBD_DATA_PORT   0x60    /* Keyboard data port */
#define KBD_STATUS_PORT 0x64    /* Keyboard status port */
#define TIMER_PORT      0x40    /* Timer port */
#define CMOS_PORT       0x70    /* CMOS port */
```

### BIOS Integration
BIOS interface definitions:
```c
/* BIOS data area definitions */
#define BIOS_DATA_AREA  0x400   /* BIOS data area base */
#define BIOS_CON_COLUMNS 0x40A  /* Console columns */
#define BIOS_CON_ROWS   0x40B  /* Console rows */

/* BIOS service interrupts */
#define BIOS_VIDEO_INT  0x10    /* Video services */
#define BIOS_DISK_INT   0x13    /* Disk services */
#define BIOS_MEMORY_INT 0x12    /* Memory size */
#define BIOS_TIME_INT   0x1A    /* Time services */
```

## x86-Specific Features

### Segmentation Support
Intel x86 segmentation model:
```c
/* x86 segment descriptors */
struct segment_descriptor {
    unsigned short sd_lolimit;  /* Segment limit (low) */
    unsigned short sd_lobase;   /* Base address (low) */
    unsigned char sd_midbase;   /* Base address (middle) */
    unsigned char sd_type;      /* Segment type */
    unsigned char sd_dpl;       /* Descriptor privilege level */
    unsigned char sd_p;         /* Segment present */
    unsigned char sd_hilimit;   /* Segment limit (high) */
    unsigned char sd_flags;     /* Flags */
    unsigned char sd_hibase;    /* Base address (high) */
};

/* Segment selector definitions */
#define USER_CS         0x1B    /* User code segment */
#define USER_DS         0x23    /* User data segment */
#define KERNEL_CS       0x08    /* Kernel code segment */
#define KERNEL_DS       0x10    /* Kernel data segment */
```

### I/O Port Access
Direct hardware I/O port operations:
```c
/* I/O port access macros */
#define inb(port)       __inbyte(port)
#define inw(port)       __inword(port)
#define inl(port)       __indword(port)
#define outb(port, val) __outbyte(port, val)
#define outw(port, val) __outword(port, val)
#define outl(port, val) __outdword(port, val)
```

### MultiProcessor Support
SMP (Symmetric MultiProcessing) definitions:
```c
/* MP configuration table */
struct mp_config_table {
    unsigned int signature;     /* "PCMP" */
    unsigned short length;      /* Table length */
    unsigned char version;      /* Specification version */
    unsigned char checksum;     /* Checksum */
    char oem_id[8];             /* OEM ID */
    char product_id[12];        /* Product ID */
    unsigned int oem_table;     /* OEM table pointer */
    unsigned short oem_size;    /* OEM table size */
    unsigned short entry_count; /* Entry count */
    unsigned int lapic_addr;    /* Local APIC address */
    unsigned short ext_length;  /* Extended table length */
    unsigned char ext_checksum; /* Extended checksum */
};
```

## Build System Integration

### Makefile Configuration
Intel x86 architecture makefile:
```makefile
# i386 architecture makefile
ARCHDIR = ${.CURDIR}
.include "${.CURDIR}/../Makefile.inc"

# Architecture-specific definitions
ARCH = i386
MACHINE_ARCH = i386
MACHINE = i386

# i386-specific compiler flags
CFLAGS += -m32 -march=i386
```

### Cross-Compilation Support
x86 cross-compilation configuration:
```makefile
# i386 cross-compilation toolchain
TOOLDIR = ${OBJDIR}/tools
CROSS_COMPILE = ${TOOLDIR}/bin/i386--netbsd-

# Architecture-specific tools
AS = ${CROSS_COMPILE}as
CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}ld
```

## ABI Conventions

### Calling Convention
Intel x86 calling convention support:
- **Stack-based arguments** - Arguments passed on stack
- **Register preservation** - Caller/callee saved register rules
- **Stack alignment** - 4-byte stack alignment requirements
- **Return values** - EAX register for integer returns
- **Floating point** - FPU stack for floating-point returns

### Stack Layout
x86 stack frame definitions:
```c
/* x86 stack frame layout */
struct trapframe {
    unsigned int tf_gs;     /* G segment register */
    unsigned int tf_fs;     /* F segment register */
    unsigned int tf_es;     /* Extra segment */
    unsigned int tf_ds;     /* Data segment */
    unsigned int tf_edi;    /* Destination index */
    unsigned int tf_esi;    /* Source index */
    unsigned int tf_ebp;    /* Base pointer */
    unsigned int tf_esp;    /* Stack pointer */
    unsigned int tf_ebx;    /* Base register */
    unsigned int tf_edx;    /* Data register */
    unsigned int tf_ecx;    /* Counter register */
    unsigned int tf_eax;    /* Accumulator register */
    unsigned int tf_trapno; /* Trap number */
    unsigned int tf_err;    /* Error code */
    unsigned int tf_eip;    /* Instruction pointer */
    unsigned int tf_cs;     /* Code segment */
    unsigned int tf_eflags; /* Flags register */
    unsigned int tf_esp;    /* Stack pointer (user) */
    unsigned int tf_ss;     /* Stack segment (user) */
};
```

## Memory Model

### Segmented Memory Model
Intel x86 segmented memory support:
- **Global Descriptor Table** - System-wide segment definitions
- **Local Descriptor Table** - Process-specific segment definitions
- **Segment Protection** - Hardware-enforced memory protection
- **Privilege Levels** - Four-level privilege model (ring 0-3)

### Flat Memory Model
Modern x86 flat memory support:
```c
/* Flat memory model definitions */
#define FLAT_CODE_SEL   0x08    /* Flat code segment */
#define FLAT_DATA_SEL   0x10    /* Flat data segment */
#define USER_CODE_SEL   0x1B    /* User code segment */
#define USER_DATA_SEL   0x23    /* User data segment */
```

## Integration with System Headers

### System-Wide Availability
Intel x86 headers are integrated into the main MINIX header system:
- **Automatic inclusion** in system builds
- **Cross-architecture compatibility** through conditional compilation
- **Platform-specific optimizations** for x86 hardware
- **Backward compatibility** with existing x86 MINIX systems

### Kernel Integration
x86 headers provide essential definitions for:
- **Real mode to protected mode transition**
- **Interrupt and exception handling**
- **Memory management unit operations**
- **Process context switching**
- **Device driver development**

## Testing and Validation

### Architecture Compatibility Testing
Intel x86 header validation includes:
- **Compile-time verification** for x86 targets
- **Runtime validation** on x86 hardware
- **QEMU testing** for virtualized environments
- **Hardware testing** on physical x86 systems

### Platform-Specific Testing
x86-specific testing ensures:
- **BIOS compatibility** across different systems
- **MultiProcessor support** validation
- **Device driver compatibility** testing
- **Performance optimization** verification

The Intel x86 architecture headers provide the essential foundation that enables MINIX 3 to operate correctly on traditional PC hardware while maintaining compatibility with the broader MINIX system architecture and supporting the unique features of the x86 instruction set and PC platform.