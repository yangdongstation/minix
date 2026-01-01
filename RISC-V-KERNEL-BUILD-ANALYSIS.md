# Minix RISC-V Kernel Build Dependencies Analysis

## Executive Summary

The Minix RISC-V kernel build has specific dependencies that must be satisfied in the correct order. The kernel depends on several libraries that need to be built first, particularly:

1. **libtimers** - Timer management functions
2. **libsys** - System call interface and kernel utilities  
3. **libexec** - Execution environment support
4. **libminc** - Minimal C library for kernel use
5. **libc** - Standard C library
6. **libcsu** - C runtime startup (crt0.o)

## Build Order Analysis

### Standard Minix Build Sequence

From `/home/donz/minix/Makefile`, the standard build order is:

```
BUILDTARGETS+= check-tools
BUILDTARGETS+= cleandir
BUILDTARGETS+= do-top-obj
BUILDTARGETS+= do-tools-obj  
BUILDTARGETS+= do-tools
BUILDTARGETS+= params
BUILDTARGETS+= obj
BUILDTARGETS+= do-distrib-dirs
BUILDTARGETS+= includes
BUILDTARGETS+= do-lib          ← Libraries built here
BUILDTARGETS+= do-build        ← Kernel built here
```

### Library Dependencies

From `/home/donz/minix/minix/kernel/Makefile`:

```makefile
LDADD+= -ltimers -lsys -lexec
```

The kernel explicitly links against these three libraries.

### Architecture-Specific Dependencies

From `/home/donz/minix/minix/kernel/arch/riscv64/Makefile.inc`:

The RISC-V kernel pulls in objects from multiple sources:

1. **libsys objects**: `assert.o`, `stacktrace.o`
2. **libminc objects**: `atoi.o`, `printf.o`, `strcmp.o`, `strcpy.o`, `strlen.o`, etc.
3. **libc objects**: `memcpy.o`, `memmove.o`, `memset.o` from `/common/lib/libc`
4. **BSP objects**: Platform-specific code from `bsp/virt/`

### Missing Components Analysis

#### 1. crt0.o (C Runtime Startup)
- **Location**: `/home/donz/minix/lib/csu/arch/riscv/crt0.S`
- **Purpose**: C runtime startup code
- **Build target**: `lib/csu`
- **Issue**: This is part of the standard C library startup, not kernel-specific

#### 2. _cpufeature.c
- **Location**: `/home/donz/minix/minix/lib/libc/arch/riscv64/_cpufeature.c`
- **Purpose**: CPU feature detection for RISC-V
- **Build target**: `minix/lib/libc`
- **Status**: Exists and should be built with libc

#### 3. Library Dependencies

The kernel needs these libraries built in order:

1. **libtimers** (`minix/lib/libtimers`) - Simple timer library
2. **libsys** (`minix/lib/libsys`) - System interface (depends on libtimers)
3. **libexec** (`minix/lib/libexec`) - Execution support
4. **libminc** (`minix/lib/libminc`) - Minimal C library (depends on libsys, libc)
5. **libc** (`lib/libc`) - Standard C library
6. **libcsu** (`lib/csu`) - C runtime startup

## RISC-V Specific Build Requirements

### Architecture Support Files

From the RISC-V README and Makefiles:

1. **CPU Feature Detection**: `_cpufeature.c` provides RISC-V feature detection
2. **Assembly Files**: Various `.S` files for context switching, exceptions, etc.
3. **BSP Support**: Board Support Package for QEMU virt platform
4. **Linker Script**: `kernel.lds` for proper kernel linking

### Memory Layout

The RISC-V kernel uses:
- **Virtual Memory**: Sv39 (39-bit virtual addresses, 3-level page tables)
- **Privilege Levels**: M-mode (OpenSBI) → S-mode (MINIX kernel) → U-mode (userspace)
- **Memory Map**: QEMU virt platform specific addresses

## Recommended Build Approach

### Option 1: Full Build (Recommended)
```bash
./build.sh -m evbriscv64 distribution
```

This builds everything in the correct order automatically.

### Option 2: Incremental Build
If you need to build components incrementally:

```bash
# 1. Build tools first
./build.sh -m evbriscv64 tools

# 2. Build libraries (in dependency order)
make -C lib/csu
make -C lib/libc
make -C minix/lib/libtimers
make -C minix/lib/libsys
make -C minix/lib/libexec
make -C minix/lib/libminc

# 3. Build includes
make includes

# 4. Build kernel
make -C minix/kernel
```

### Option 3: Kernel-Only Build (Advanced)
If libraries are already built:

```bash
# Set up environment
export MACHINE=evbriscv64
export MACHINE_ARCH=riscv64
export DESTDIR=/path/to/destdir

# Build just the kernel dependencies
make -C minix/lib/libtimers
make -C minix/lib/libsys  
make -C minix/lib/libexec

# Build kernel
make -C minix/kernel
```

## Key Findings

1. **No Missing Source Code**: All required components exist in the source tree
2. **Proper Dependencies**: The build system has correct dependency declarations
3. **Architecture Support**: RISC-V 64-bit support is complete with proper BSP
4. **Build Order Critical**: Libraries must be built before the kernel

## Common Issues and Solutions

### Issue: "cannot find -ltimers"
**Solution**: Build `minix/lib/libtimers` first

### Issue: "cannot find -lsys"  
**Solution**: Build `minix/lib/libsys` after libtimers

### Issue: "cannot find -lexec"
**Solution**: Build `minix/lib/libexec` after libsys

### Issue: Missing crt0.o
**Solution**: This is expected for kernel builds - crt0.o is for userspace programs, not kernels

## Conclusion

The Minix RISC-V kernel build system is properly designed with the correct dependencies. The issue is not missing source code but ensuring the proper build order is followed. The standard `./build.sh -m evbriscv64 distribution` command handles all dependencies automatically and is the recommended approach.