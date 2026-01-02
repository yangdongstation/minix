# MINIX 3 RISC-V 64-bit Port

## Overview

This directory contains the RISC-V 64-bit (RV64GC) architecture support for MINIX 3.
The port targets the QEMU `virt` machine platform.

## Target Platform

- **Architecture**: RISC-V 64-bit (RV64GC)
- **ISA Extensions**: IMAFDC (Integer, Multiply, Atomic, Float, Double, Compressed)
- **Privilege Levels**: M-mode (OpenSBI) + S-mode (MINIX kernel) + U-mode (userspace)
- **Virtual Memory**: Sv39 (39-bit virtual address, 3-level page tables)
- **Target Machine**: QEMU virt platform

## Directory Structure

```
riscv64/
├── include/           # Architecture-specific headers
│   ├── archconst.h    # Constants (memory map, CSR bits)
│   ├── arch_proto.h   # Function prototypes, inline CSR ops
│   ├── arch_clock.h   # Clock/timer definitions
│   ├── hw_intr.h      # Hardware interrupt definitions
│   └── sconst.h       # Assembly constants
├── bsp/               # Board Support Package (QEMU virt)
│   ├── bsp_init.c     # Platform initialization
│   ├── bsp_serial.c   # UART output
│   ├── bsp_timer.c    # Timer setup
│   ├── bsp_reset.c    # System reset
│   └── bsp_intr.c     # Interrupt controller
├── head.S             # Boot entry point
├── mpx.S              # Context switch assembly
├── exc.S              # Exception/interrupt entry
├── klib.S             # Kernel library (memcpy, etc.)
├── phys_copy.S        # Physical memory copy
├── phys_memset.S      # Physical memory set
├── sbi.c              # SBI (Supervisor Binary Interface)
├── plic.c             # PLIC interrupt controller driver
├── exception.c        # Exception handling
├── arch_clock.c       # Timer/clock management
├── arch_system.c      # System management (shutdown, reboot)
├── memory.c           # Memory management
├── protect.c          # Protection/privilege setup
├── pg_utils.c         # Page table utilities
├── kernel.lds         # Linker script
└── Makefile.inc       # Build configuration
```

## Memory Map (QEMU virt)

| Address Range         | Size    | Description           |
|-----------------------|---------|-----------------------|
| 0x00000000-0x00010000 | 64KB    | Boot ROM              |
| 0x02000000-0x02010000 | 64KB    | CLINT (timer/IPI)     |
| 0x0C000000-0x10000000 | 64MB    | PLIC                  |
| 0x10000000-0x10000100 | 256B    | UART (NS16550)        |
| 0x10001000-0x10008000 | 28KB    | VirtIO MMIO devices   |
| 0x80000000-...        | Variable| RAM                   |

## Key Components

### SBI Interface (sbi.c)
- Timer management via SBI_EXT_TIME
- Console I/O via legacy putchar
- Inter-processor interrupts via SBI_EXT_IPI
- System reset via SBI_EXT_SRST

### PLIC Driver (plic.c)
- External interrupt routing
- Per-CPU interrupt enable/disable
- Priority management
- SMP support via `plic_irq_cpu_mask()`

### Page Tables (Sv39)
- 3-level page tables
- 4KB pages (2MB/1GB superpages supported)
- 512 entries per level (9 bits per level)
- Physical address: bits 55:12 in PTE

## Building

```bash
# Build toolchain
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no ./build.sh -U -m evbriscv64 tools

# Build distribution (current working set)
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
./build.sh -j$(nproc) -m evbriscv64 \
  -V AVAILABLE_COMPILER=gcc -V ACTIVE_CC=gcc -V ACTIVE_CPP=gcc -V ACTIVE_CXX=gcc -V ACTIVE_OBJC=gcc \
  -V RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany' \
  -V NOGCCERROR=yes \
  -V MKPIC=no -V MKPICLIB=no -V MKPICINSTALL=no \
  -V MKCXX=no -V MKLIBSTDCXX=no -V MKATF=no \
  -V USE_PCI=no \
  -V CHECKFLIST_FLAGS='-m -e' \
  distribution
```

Notes:
- If your toolchain supports `-march=rv64gc -mabi=lp64d`, drop `RISCV_ARCH_FLAGS`.
- `CHECKFLIST_FLAGS='-m -e'` allows missing/extra files while the sets are incomplete.

## Running

### QEMU
```bash
# Basic run
./minix/scripts/qemu-riscv64.sh -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64

# Debug mode (wait for GDB)
./minix/scripts/qemu-riscv64.sh -d -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64

# With networking
./minix/scripts/qemu-riscv64.sh -n -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64
```

### GDB Debugging
```bash
# Terminal 1: Start QEMU in debug mode
./minix/scripts/qemu-riscv64.sh -d -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64

# Terminal 2: Connect GDB
./minix/scripts/gdb-riscv64.sh minix/kernel/obj/kernel
```

## Boot Sequence

1. **OpenSBI** (M-mode): Initializes hardware, provides SBI services
2. **head.S**: Sets up initial page tables, enables Sv39, jumps to kernel_main
3. **kernel_main**: Initializes MINIX kernel, starts servers
4. **init**: First userspace process

## CSR Usage

| CSR     | Purpose                            |
|---------|------------------------------------|
| sstatus | Supervisor status (SIE, SPP, etc.) |
| sie     | Supervisor interrupt enable        |
| sip     | Supervisor interrupt pending       |
| stvec   | Supervisor trap vector             |
| sepc    | Supervisor exception PC            |
| scause  | Supervisor trap cause              |
| stval   | Supervisor trap value              |
| satp    | Supervisor address translation     |
| sscratch| Supervisor scratch register        |

## Interrupt Handling

- **Software interrupts**: IPI via SBI
- **Timer interrupts**: Via SBI timer extension
- **External interrupts**: Via PLIC

## TODO / Known Issues

1. SMP support is scaffolded but not fully tested
2. FPU context switching needs implementation
3. Debug registers (hardware breakpoints) not yet supported

## References

- [RISC-V Privileged Specification](https://riscv.org/specifications/privileged-isa/)
- [RISC-V SBI Specification](https://github.com/riscv-non-isa/riscv-sbi-doc)
- [QEMU RISC-V virt Platform](https://www.qemu.org/docs/master/system/riscv/virt.html)
