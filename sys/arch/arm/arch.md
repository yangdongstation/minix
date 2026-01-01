# ARM Architecture System Headers

## Overview

The `sys/arch/arm/` directory contains architecture-specific system headers and definitions for ARM processors in MINIX 3. This directory provides the essential header files that define ARM-specific constants, data structures, and interfaces required for system-level programming on ARM-based embedded systems and evaluation boards.

## Purpose and Responsibilities

### Core Functions
- **ARM Architecture Support**: Provides comprehensive ARM processor support (ARMv4, ARMv5, ARMv6, ARMv7)
- **Embedded Systems Integration**: Defines interfaces for ARM-based embedded platforms
- **Multiple ARM Variants**: Supports various ARM cores (ARM7, ARM9, ARM11, Cortex-A, Cortex-M)
- **Evaluation Board Support**: Provides platform-specific definitions for ARM evaluation boards
- **Power Management**: Includes ARM-specific power management interfaces

### Key Features
- **Multiple Instruction Sets** - ARM, Thumb, Thumb-2 support
- **Memory Management Unit** - ARM MMU with different page sizes
- **Interrupt Controllers** - VIC, GIC, and platform-specific interrupt handling
- **Cache Management** - ARM cache maintenance operations
- **Co-processor Interface** - ARM co-processor access mechanisms

## Directory Structure

```
sys/arch/arm/
├── include/                 # Architecture-specific header files
│   ├── Makefile           # Header build configuration
│   ├── arch/              # Core architecture definitions
│   │   ├── _bus.h         # Bus space definitions
│   │   ├── _conf.h        # Configuration space definitions
│   │   ├── _types.h       # Architecture types
│   │   ├── atomic.h       # Atomic operation definitions
│   │   ├── atomic_r6.h    # ARMv6 atomic operations
│   │   ├── autoconf.h     # Auto-configuration definitions
│   │   ├── beagle.h       # BeagleBoard specific
│   │   ├── bootinfo.h     # Boot information structures
│   │   ├── bus.h          # Bus interface definitions
│   │   ├── bus_space.h    # Bus space operations
│   │   ├── cdefs.h        # C compiler definitions
│   │   ├── conf.h         # Configuration definitions
│   │   ├── cpu.h          # CPU-specific definitions
│   │   ├── db_machdep.h   # Machine-dependent debugger
│   │   ├── disklabel.h    # Disk label definitions
│   │   ├── endian.h       # Endianness definitions
│   │   ├── endian_machdep.h # Machine-dependent endian
│   │   ├── footprint.h    # Memory footprint definitions
│   │   ├── frame.h        # Stack frame definitions
│   │   ├── hpcarm_mfpr.h  # HPC ARM MFP register
│   │   ├── hpc.h          # Handheld PC definitions
│   │   ├── ieeefp.h       # IEEE floating point
│   │   ├── inline.h       # Inline assembly definitions
│   │   ├── intr.h         # Interrupt handling definitions
│   │   ├── iomd.h         # IOMD definitions
│   │   ├── iq80321.h      # IQ80321 specific
│   │   ├── irq.h          # IRQ definitions
│   │   ├── ixpd425.h      # IXPD425 specific
│   │   ├── kbd_machdep.h  # Keyboard machine-dependent
│   │   ├── lock.h         # Lock primitive definitions
│   │   ├── machdep.h      # Machine-dependent definitions
│   │   ├── mutex.h        # Mutex implementation
│   │   ├── mutex_r6.h     # ARMv6 mutex implementation
│   │   ├── param.h        # Architecture parameters
│   │   ├── pcb.h          # Process Control Block
│   │   ├── pmap.h         # Physical map definitions
│   │   ├── proc.h         # Process structure definitions
│   │   ├── profile.h      # Profiling support
│   │   ├── ptrace.h       # Process tracing definitions
│   │   ├── pxa2x0.h       # PXA2x0 specific
│   │   ├── pxa270.h       # PXA270 specific
│   │   ├── reg.h          # Register definitions
│   │   ├── reloc.h        # Relocation definitions
│   │   ├── s3c2410var.h   # S3C2410 variables
│   │   ├── s3c2410.h      # S3C2410 specific
│   │   ├── s3c24x0.h      # S3C24x0 specific
│   │   ├── s3c2800.h      # S3C2800 specific
│   │   ├── setjmp.h       # Non-local jump definitions
│   │   ├── signal.h       # Signal handling definitions
│   │   ├── stack.h        # Stack definitions
│   │   ├── stdarg.h       # Standard argument handling
│   │   ├── sysarch.h      # System architecture calls
│   │   ├── trap.h         # Trap/exception definitions
│   │   ├── types.h        # Architecture types
│   │   ├── varargs.h      # Variable argument handling
│   │   ├── vmparam.h      # Virtual memory parameters
│   │   └── wzero3.h       # WZERO3 specific
│   ├── arm32_machdep.h    # ARM32 machine-dependent
│   ├── armreg.h           # ARM register definitions
│   ├── atomic.h           # Atomic operations header
│   ├── beagle.h           # BeagleBoard header
│   ├── bootconfig.h       # Boot configuration
│   ├── bus_defs.h         # Bus definition constants
│   ├── bus.h              # Bus header
│   ├── cdefs.h            # C definitions
│   ├── conf.h             # Configuration header
│   ├── cpu.h              # CPU header
│   ├── db_machdep.h       # Debugger machine-dependent
│   ├── disklabel.h        # Disk label header
│   ├── elf_machdep.h      # ELF format dependencies
│   ├── endian.h           # Endianness definitions
│   ├── endian_machdep.h   # Machine-dependent endian
│   ├── footprint.h        # Memory footprint header
│   ├── frame.h            # Stack frame header
│   ├── hpcarm_mfpr.h      # HPC ARM MFP register header
│   ├── hpc.h              # Handheld PC header
│   ├── ieee.h             # IEEE floating point
│   ├── ieee754.h          # IEEE 754 floating point
│   ├── ieeefp.h           # IEEE FP definitions
│   ├── intr.h             # Interrupt header
│   ├── iomd.h             # IOMD header
│   ├── iq80321.h          # IQ80321 header
│   ├── ixpd425.h          # IXPD425 header
│   ├── limits.h           # System limits
│   ├── locking.h          # Locking definitions
│   ├── machTypes.h        # Machine types
│   ├── mcontext.h         # Machine context
│   ├── mutex.h            # Mutex header
│   ├── param.h            # System parameters
│   ├── pcb.h              # Process control block
│   ├── pmap.h             # Physical map
│   ├── proc.h             # Process definitions
│   ├── profile.h          # Profiling header
│   ├── pxa2x0.h           # PXA2x0 header
│   ├── pxa270.h           # PXA270 header
│   ├── ptrace.h           # Process tracing
│   ├── reg.h              # Register header
│   ├── s3c2410.h          # S3C2410 header
│   ├── s3c24x0.h          # S3C24x0 header
│   ├── s3c2800.h          # S3C2800 header
│   ├── setjmp.h           # Non-local jumps
│   ├── signal.h           # Signal handling
│   ├── specialreg.h       # Special registers
│   ├── stack.h            # Stack header
│   ├── stdarg.h           # Standard arguments
│   ├── trap.h             # Trap definitions
│   ├── types.h            # Type definitions
│   ├── varargs.h          # Variable arguments
│   ├── vmparam.h          # Virtual memory parameters
│   └── wzero3.h           # WZERO3 header
├── Makefile               # Main architecture makefile
├── Makefile.inc           # Architecture include makefile
└── conf/                  # Configuration files
    ├── GENERIC            # Generic ARM configuration
    ├── GENERIC.local      # Local ARM modifications
    ├── GENERIC_IPS        # Generic with IP security
    ├── GENERIC_IPS.local  # IPS local modifications
    ├── IQ31244            # IQ31244 specific
    ├── IQ31244.local      # IQ31244 local
    ├── IQ80321            # IQ80321 specific
    ├── IQ80321.local      # IQ80321 local
    ├── IXCDT1100          # IXCDT1100 specific
    ├── IXCDT1100.local    # IXCDT1100 local
    ├── IXDP425            # IXDP425 specific
    ├── IXDP425.local      # IXDP425 local
    ├── Makefile.evbarm    # Evaluation board ARM makefile
    ├── Makefile.rpi       # Raspberry Pi makefile
    ├── RPI                # Raspberry Pi configuration
    ├── RPI.local          # Raspberry Pi local
    ├── S3C2410            # S3C2410 specific
    ├── S3C2410.local      # S3C2410 local
    └── WZERO3             # WZERO3 specific
```

## ARM Architecture-Specific Definitions

### Architecture Parameters (`include/arch/param.h`)
```c
/* ARM architecture parameters */
#define MACHINE_ARCH        "arm"
#define MACHINE             "arm"
#define PAGE_SHIFT          12              /* 4KB pages */
#define PAGE_SIZE           (1UL << PAGE_SHIFT)
#define PAGE_MASK           (PAGE_SIZE - 1)
#define NBPG                PAGE_SIZE       /* Bytes per page */
#define PGSHIFT             PAGE_SHIFT      /* LOG2(PAGE_SIZE) */

/* ARM-specific parameters */
#define ARM_PAGE_SHIFT      12
#define ARM_MAXADDR         0xFFFFFFFF      /* 4GB address space */
#define ARM_MAXUSERADDR     0xBFFFFFFF      /* 3GB user space */

/* ARM processor modes */
#define ARM_MODE_USR        0x10    /* User mode */
#define ARM_MODE_FIQ        0x11    /* FIQ mode */
#define ARM_MODE_IRQ        0x12    /* IRQ mode */
#define ARM_MODE_SVC        0x13    /* Supervisor mode */
#define ARM_MODE_ABT        0x17    /* Abort mode */
#define ARM_MODE_UND        0x1B    /* Undefined mode */
#define ARM_MODE_SYS        0x1F    /* System mode */
```

### Register Definitions (`include/arch/reg.h`)
ARM register layout and conventions:
```c
/* ARM general purpose registers */
struct reg {
    unsigned int r_r0;      /* Register 0 */
    unsigned int r_r1;      /* Register 1 */
    unsigned int r_r2;      /* Register 2 */
    unsigned int r_r3;      /* Register 3 */
    unsigned int r_r4;      /* Register 4 */
    unsigned int r_r5;      /* Register 5 */
    unsigned int r_r6;      /* Register 6 */
    unsigned int r_r7;      /* Register 7 */
    unsigned int r_r8;      /* Register 8 */
    unsigned int r_r9;      /* Register 9 */
    unsigned int r_r10;     /* Register 10 */
    unsigned int r_r11;     /* Frame pointer */
    unsigned int r_r12;     /* IP register */
    unsigned int r_r13;     /* Stack pointer */
    unsigned int r_r14;     /* Link register */
    unsigned int r_r15;     /* Program counter */
    unsigned int r_cpsr;    /* Current program status register */
};

/* ARM co-processor registers */
struct cp15_reg {
    unsigned int c0;        /* ID codes */
    unsigned int c1;        /* Control register */
    unsigned int c2;        /* Translation table base */
    unsigned int c3;        /* Domain access control */
    unsigned int c5;        /* Fault status */
    unsigned int c6;        /* Fault address */
    unsigned int c7;        /* Cache operations */
    unsigned int c8;        /* TLB operations */
    unsigned int c9;        /* Cache lockdown */
    unsigned int c10;       /* TLB lockdown */
    unsigned int c13;       /* Process ID */
    unsigned int c15;       /* Test configuration */
};
```

### Virtual Memory Parameters (`include/arch/vmparam.h`)
ARM memory management definitions:
```c
/* ARM virtual memory parameters */
#define ARM_VM_MIN_ADDRESS      0x00000000
#define ARM_VM_MAXUSER_ADDRESS  0xBFFFFFFF  /* 3GB user space */
#define ARM_VM_MAX_ADDRESS      0xFFFFFFFF  /* 4GB total space */

/* ARM page table structures */
#define ARM_L1_SHIFT            20          /* L1 table shift */
#define ARM_L2_SHIFT            12          /* L2 table shift */
#define ARM_L1_SIZE             0x00100000  /* 1MB sections */
#define ARM_L2_SIZE             0x00001000  /* 4KB pages */
#define ARM_L1_ENTRIES          4096        /* L1 table entries */
#define ARM_L2_ENTRIES          256         /* L2 table entries */

/* ARM page protection flags */
#define ARM_L1_TYPE_MASK        0x00000003  /* L1 entry type */
#define ARM_L1_TYPE_FAULT       0x00000000  /* Fault entry */
#define ARM_L1_TYPE_PAGE        0x00000001  /* Page table entry */
#define ARM_L1_TYPE_SECTION     0x00000002  /* Section entry */
#define ARM_L1_AP_MASK          0x00000C00  /* Access permission */
#define ARM_L1_AP_RW            0x00000400  /* Read/write */
#define ARM_L1_AP_RO            0x00000800  /* Read only */
```

### Trap Definitions (`include/arch/trap.h`)
ARM exception and interrupt handling:
```c
/* ARM exception codes */
#define ARM_EXCEPTION_RESET     0   /* Reset */
#define ARM_EXCEPTION_UNDEF     1   /* Undefined instruction */
#define ARM_EXCEPTION_SWI       2   /* Software interrupt */
#define ARM_EXCEPTION_PREFETCH  3   /* Prefetch abort */
#define ARM_EXCEPTION_DATA      4   /* Data abort */
#define ARM_EXCEPTION_RESERVED  5   /* Reserved */
#define ARM_EXCEPTION_IRQ       6   /* IRQ */
#define ARM_EXCEPTION_FIQ       7   /* FIQ */

/* ARM interrupt vectors */
#define ARM_IRQ_BASE            32  /* Base of IRQ vectors */
#define ARM_FIQ_BASE            96  /* Base of FIQ vectors */

/* ARM processor features */
#define ARM_FEATURE_THUMB       0x00000001  /* Thumb instruction set */
#define ARM_FEATURE_26BIT       0x00000002  /* 26-bit addressing */
#define ARM_FEATURE_VFP         0x00000004  /* Vector floating point */
#define ARM_FEATURE_NEON        0x00000008  /* NEON SIMD */
```

## Platform Integration

### ARM Evaluation Board Support
Common ARM evaluation board definitions:
```c
/* ARM evaluation board memory maps */
#define ARM_EB_MEM_BASE         0x00000000  /* Memory base */
#define ARM_EB_MEM_SIZE         0x10000000  /* 256MB memory */
#define ARM_EB_FLASH_BASE       0x40000000  /* Flash base */
#define ARM_EB_FLASH_SIZE       0x04000000  /* 64MB flash */
#define ARM_EB_PERIPH_BASE      0x80000000  /* Peripherals base */
#define ARM_EB_PERIPH_SIZE      0x40000000  /* 1GB peripherals */
```

### ARM Interrupt Controller
ARM interrupt controller interfaces:
```c
/* Vectored Interrupt Controller (VIC) */
#define VIC_BASE                0x10140000  /* VIC base address */
#define VIC_IRQSTATUS           0x00        /* IRQ status */
#define VIC_FIQSTATUS           0x04        /* FIQ status */
#define VIC_RAWINTR             0x08        /* Raw interrupt status */
#define VIC_INTSELECT           0x0C        /* Interrupt select */
#define VIC_INTENABLE           0x10        /* Interrupt enable */
#define VIC_INTENCLEAR          0x14        /* Interrupt enable clear */
#define VIC_SOFTINT             0x18        /* Software interrupt */
#define VIC_SOFTINTCLEAR        0x1C        /* Software interrupt clear */
#define VIC_PROTECTION          0x20        /* Protection enable */
#define VIC_SWPRIORITY          0x24        /* Software priority mask */
#define VIC_PRIORITYDAISY       0x28        /* Priority daisy */
#define VIC_VECTADDR0           0x100       /* Vector address 0 */
```

## ARM-Specific Features

### Multiple Instruction Sets
ARM instruction set support:
```c
/* ARM instruction set definitions */
#define ARM_STATE_ARM           0   /* ARM instruction set */
#define ARM_STATE_THUMB         1   /* Thumb instruction set */
#define ARM_STATE_JAZELLE       2   /* Jazelle instruction set */

/* Thumb instruction decoding */
#define THUMB_INSN_SIZE         2   /* Thumb instruction size */
#define ARM_INSN_SIZE           4   /* ARM instruction size */
```

### Cache Management
ARM cache maintenance operations:
```c
/* ARM cache operations */
#define ARM_CACHE_ICACHE        0x01    /* Instruction cache */
#define ARM_CACHE_DCACHE        0x02    /* Data cache */
#define ARM_CACHE_UNIFIED       0x04    /* Unified cache */

/* Cache maintenance operations */
#define ARM_CACHE_CLEAN         0x01    /* Clean cache */
#define ARM_CACHE_INVALIDATE    0x02    /* Invalidate cache */
#define ARM_CACHE_FLUSH         0x03    /* Clean and invalidate */
```

### Co-processor Interface
ARM co-processor access mechanisms:
```c
/* ARM co-processor definitions */
#define ARM_CP15                15  /* System control co-processor */
#define ARM_CP14                14  /* Debug co-processor */
#define ARM_CP10                10  /* VFP co-processor */
#define ARM_CP11                11  /* VFP co-processor */

/* CP15 register operations */
#define CP15_READ(reg, cp, op1, crn, crm, op2) \
    __asm__ volatile("mrc p" #cp ", " #op1 ", %0, c" #crn ", c" #crm ", " #op2 \
                     : "=r" (reg))

#define CP15_WRITE(reg, cp, op1, crn, crm, op2) \
    __asm__ volatile("mcr p" #cp ", " #op1 ", %0, c" #crn ", c" #crm ", " #op2 \
                     : : "r" (reg))
```

## Platform-Specific Support

### BeagleBoard Support
BeagleBoard platform definitions:
```c
/* BeagleBoard specific definitions */
#define BEAGLEBOARD_OMAP3       1   /* OMAP3 processor */
#define BEAGLEBOARD_SDRAM_SIZE  0x10000000  /* 256MB SDRAM */
#define BEAGLEBOARD_NAND_SIZE   0x04000000  /* 64MB NAND */

/* BeagleBoard memory map */
#define BEAGLEBOARD_SDRAM_BASE  0x80000000  /* SDRAM base */
#define BEAGLEBOARD_NAND_BASE   0x04000000  /* NAND flash base */
#define BEAGLEBOARD_PERIPH_BASE 0x48000000  /* Peripherals base */
```

### Raspberry Pi Support
Raspberry Pi platform definitions:
```c
/* Raspberry Pi specific definitions */
#define RPI_BCM2835             1   /* BCM2835 processor */
#define RPI_SDRAM_SIZE          0x40000000  /* 1GB SDRAM */
#define RPI_GPU_MEM_SIZE        0x04000000  /* 64MB GPU memory */

/* Raspberry Pi memory map */
#define RPI_SDRAM_BASE          0x00000000  /* SDRAM base */
#define RPI_PERIPH_BASE         0x20000000  /* Peripherals base */
#define RPI_ARM_BASE            0x20000000  /* ARM peripherals */
#define RPI_VC_BASE             0x40000000  /* VideoCore peripherals */
```

## Build System Integration

### Makefile Configuration
ARM architecture makefile:
```makefile
# ARM architecture makefile
ARCHDIR = ${.CURDIR}
.include "${.CURDIR}/../Makefile.inc"

# Architecture-specific definitions
ARCH = arm
MACHINE_ARCH = arm
MACHINE = arm

# ARM-specific compiler flags
CFLAGS += -marm -mapcs-frame
```

### Cross-Compilation Support
ARM cross-compilation configuration:
```makefile
# ARM cross-compilation toolchain
TOOLDIR = ${OBJDIR}/tools
CROSS_COMPILE = ${TOOLDIR}/bin/arm--netbsd-

# Architecture-specific tools
AS = ${CROSS_COMPILE}as
CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}ld
```

## ABI Conventions

### ARM Architecture Procedure Call Standard (AAPCS)
ARM calling convention support:
- **Register usage** - r0-r3 for arguments, r0-r1 for return values
- **Stack alignment** - 8-byte stack alignment requirements
- **Frame pointer** - r11 as frame pointer
- **Link register** - r14 as link register
- **Stack pointer** - r13 as stack pointer

### Stack Layout
ARM stack frame definitions:
```c
/* ARM stack frame layout */
struct trapframe {
    unsigned int tf_r0;     /* Register 0 */
    unsigned int tf_r1;     /* Register 1 */
    unsigned int tf_r2;     /* Register 2 */
    unsigned int tf_r3;     /* Register 3 */
    unsigned int tf_r4;     /* Register 4 */
    unsigned int tf_r5;     /* Register 5 */
    unsigned int tf_r6;     /* Register 6 */
    unsigned int tf_r7;     /* Register 7 */
    unsigned int tf_r8;     /* Register 8 */
    unsigned int tf_r9;     /* Register 9 */
    unsigned int tf_r10;    /* Register 10 */
    unsigned int tf_r11;    /* Frame pointer */
    unsigned int tf_r12;    /* IP register */
    unsigned int tf_r13;    /* Stack pointer */
    unsigned int tf_r14;    /* Link register */
    unsigned int tf_r15;    /* Program counter */
    unsigned int tf_spsr;   /* Saved program status register */
};
```

## Memory Model

### ARM Memory Model
ARM memory model support:
- **Weak ordering** - ARM weak memory ordering model
- **Memory barriers** - Data synchronization barrier instructions
- **Cache coherency** - Multi-core cache coherency support
- **Memory protection** - ARM memory protection unit

### Virtual Memory Integration
ARM virtual memory support:
```c
/* ARM memory domain definitions */
#define ARM_DOMAIN_NO_ACCESS    0   /* No access */
#define ARM_DOMAIN_CLIENT       1   /* Client access */
#define ARM_DOMAIN_RESERVED     2   /* Reserved */
#define ARM_DOMAIN_MANAGER      3   /* Manager access */

/* ARM memory type definitions */
#define ARM_MEMTYPE_STRONGLY_ORDERED    0   /* Strongly ordered */
#define ARM_MEMTYPE_DEVICE              1   /* Device memory */
#define ARM_MEMTYPE_NORMAL              2   /* Normal memory */
```

## Integration with System Headers

### System-Wide Availability
ARM headers are integrated into the main MINIX header system:
- **Automatic inclusion** in system builds
- **Cross-architecture compatibility** through conditional compilation
- **Platform-specific optimizations** for ARM hardware
- **Evaluation board support** for development platforms

### Kernel Integration
ARM headers provide essential definitions for:
- **Boot loader integration** with ARM platforms
- **Interrupt controller initialization**
- **Memory management unit setup**
- **Process context switching**
- **Device driver development** for ARM peripherals

## Testing and Validation

### Architecture Compatibility Testing
ARM header validation includes:
- **Compile-time verification** for ARM targets
- **Runtime validation** on ARM hardware
- **QEMU testing** for virtualized environments
- **Hardware testing** on evaluation boards

### Platform-Specific Testing
ARM-specific testing ensures:
- **Evaluation board compatibility** across different platforms
- **Multi-core support** validation
- **Cache coherency** testing
- **Power management** verification

The ARM architecture headers provide the essential foundation that enables MINIX 3 to operate correctly on ARM-based embedded systems while maintaining compatibility with the broader MINIX system architecture and supporting the unique features of the ARM instruction set architecture and embedded platforms.