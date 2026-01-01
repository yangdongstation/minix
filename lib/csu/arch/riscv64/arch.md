# RISC-V 64-bit C Startup Code Architecture

## Overview

The `lib/csu/arch/riscv64/` directory contains the C startup code (C Runtime Start-up) for RISC-V 64-bit architecture. This directory provides the essential runtime initialization code that executes before `main()` is called, including program entry points, constructor/destructor handling, and runtime environment setup.

## Purpose and Responsibilities

### Core Startup Functions
- **Program Entry Point**: Initial program execution entry (`_start`)
- **Runtime Initialization**: Stack setup, argument processing, environment preparation
- **Constructor/Destructor Handling**: Global object initialization and cleanup
- **Dynamic Linking Support**: Runtime linking and symbol resolution
- **Exception Handling Setup**: Runtime exception framework initialization
- **Thread-Local Storage**: TLS initialization for multi-threaded programs

### Key Components
- **crt0**: Basic runtime startup (C Runtime 0)
- **crti**: Constructor initialization prologue
- **crtn**: Constructor/destructor epilogue
- **crt1**: Advanced runtime startup with full initialization
- **Scrt1**: Position-independent executable startup
- **gcrt1**: Profiling-enabled startup

## Directory Structure

```
lib/csu/arch/riscv64/
├── machine/                # Machine-specific headers and definitions
└── [symlink to riscv/]     # Links to riscv/ directory for shared implementation
```

## RISC-V 64-bit Startup Sequence

### Program Entry Point
```assembly
# RISC-V 64-bit program entry point (crt0.S)
.text
.globl _start
_start:
    # Initial stack pointer already set by kernel
    # argc is at sp, argv at sp+8, envp at sp+16
    
    # Save initial stack pointer
    mv s0, sp
    
    # Align stack to 16-byte boundary
    andi sp, sp, -16
    
    # Set up initial frame
    addi sp, sp, -FRAME_SIZE
    sd ra, FRAME_RA(sp)
    sd s0, FRAME_S0(sp)
    
    # Call C runtime initialization
    call _init
    
    # Set up arguments for main()
    ld a0, 0(s0)      # argc
    addi a1, s0, 8    # argv
    addi a2, s0, 16   # envp
    
    # Call main()
    call main
    
    # Exit with main's return value
    mv a0, a0         # Return code from main
    call _exit
```

### Runtime Initialization
```c
// Runtime initialization sequence
void _init(void)
{
    // Initialize runtime environment
    __environ = environ;
    
    // Initialize thread-local storage
    _tls_init();
    
    // Initialize static constructors
    _do_global_ctors_aux();
    
    // Initialize atexit handlers
    __cxa_atexit(_do_global_dtors_aux, NULL, NULL);
    
    // Platform-specific initialization
    _platform_init();
}
```

## Constructor/Destructor Framework

### Constructor Initialization (crti.S)
```assembly
# Constructor initialization prologue
.section .init
.global _init
_init:
    # Save return address and frame pointer
    addi sp, sp, -16
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi s0, sp, 16
    
    # Jump to next constructor in chain
    call _init_next

.section .fini
.global _fini
_fini:
    # Save return address and frame pointer
    addi sp, sp, -16
    sd ra, 8(sp)
    sd s0, 0(sp)
    addi s0, sp, 16
    
    # Jump to next destructor in chain
    call _fini_next
```

### Constructor Finalization (crtn.S)
```assembly
# Constructor finalization epilogue
.section .init
    # Restore frame pointer and return
    ld s0, 0(sp)
    ld ra, 8(sp)
    addi sp, sp, 16
    ret

.section .fini
    # Restore frame pointer and return
    ld s0, 0(sp)
    ld ra, 8(sp)
    addi sp, sp, 16
    ret
```

## RISC-V 64-bit Specific Optimizations

### Stack Alignment
RISC-V requires 16-byte stack alignment for proper operation:
```assembly
# Ensure 16-byte stack alignment
andi sp, sp, -16
```

### Register Save/Restore
Efficient use of RISC-V calling convention:
```assembly
# Save callee-saved registers efficiently
sd s0, 0(sp)
sd s1, 8(sp)
sd s2, 16(sp)
# ... save additional registers as needed
```

### Position-Independent Code
Support for position-independent executables:
```assembly
# Generate PC-relative addresses
auipc t0, %pcrel_hi(symbol)
addi t0, t0, %pcrel_lo(symbol)
```

## Dynamic Linking Support

### Global Offset Table (GOT) Setup
```assembly
# Initialize GOT pointer for dynamic linking
.option push
.option norelax
la gp, __global_pointer$
.option pop
```

### Procedure Linkage Table (PLT) Support
```assembly
# PLT entry for external function calls
.plt
.global printf
printf:
    auipc t3, %pcrel_hi(.got.plt)
    ld t3, %pcrel_lo(.got.plt)(t3)
    jalr t1, t3
```

## Thread-Local Storage (TLS) Initialization

### TLS Setup
```c
// Thread-local storage initialization
void _tls_init(void)
{
    extern char __tdata_start[], __tdata_end[];
    extern char __tbss_start[], __tbss_end[];
    extern char __tls_init[];
    
    size_t tdata_size = __tdata_end - __tdata_start;
    size_t tbss_size = __tbss_end - __tbss_start;
    
    // Allocate TLS block
    void *tls_block = _tls_allocate(tdata_size + tbss_size);
    
    // Copy initial TLS data
    memcpy(tls_block, __tdata_start, tdata_size);
    
    // Clear TLS BSS
    memset(tls_block + tdata_size, 0, tbss_size);
    
    // Set TLS pointer
    _set_tls_pointer(tls_block);
}
```

## Exception Handling Framework

### Frame Unwind Information
```assembly
# Exception frame setup
.cfi_startproc
.cfi_def_cfa sp, 0
.cfi_offset ra, 8
.cfi_offset s0, 0
# ... additional CFI directives
.cfi_endproc
```

### Personality Routine
```c
// Exception personality routine
int __gxx_personality_v0(int version, int actions, 
                         uint64_t exception_class,
                         void *exception_object, 
                         void *context)
{
    // Handle exception unwinding
    return _URC_CONTINUE_UNWIND;
}
```

## Build Configuration

### Makefile Configuration
```makefile
# RISC-V 64-bit CSU build configuration
RISC_V_SRCS += \
    crt0.S \
    crti.S \
    crtn.S \
    crt1.c \
    Scrt1.c \
    gcrt1.c

# Architecture-specific flags
CFLAGS += -march=rv64gc -mabi=lp64d
CFLAGS += -fPIC -fexceptions
```

### Object File Generation
```bash
# Generate startup objects
riscv64-unknown-elf-gcc -c crt0.S -o crt0.o
riscv64-unknown-elf-gcc -c crti.S -o crti.o
riscv64-unknown-elf-gcc -c crtn.S -o crtn.o
```

## Linker Integration

### Linker Script Integration
```ld
/* Linker script startup sections */
.init :
{
    KEEP (*crti.o(.init))
    KEEP (*(.init))
    KEEP (*crtn.o(.init))
}

.fini :
{
    KEEP (*crti.o(.fini))
    KEEP (*(.fini))
    KEEP (*crtn.o(.fini))
}
```

### Startup Object Ordering
```bash
# Correct linking order
ld crt1.o crti.o [user objects] crtn.o -lc -o program
```

## Performance Characteristics

### Startup Time Optimization
- Minimal initialization overhead
- Efficient register usage
- Optimized stack operations
- Fast path for common cases

### Memory Footprint
- Compact startup code
- Minimal static data requirements
- Efficient TLS implementation
- Optimized exception handling

## Debugging Support

### Debug Information
```assembly
# Debug-friendly startup code
.file "crt0.S"
.loc 1 10 0
_start:
    .cfi_startproc
    # ... startup code
    .cfi_endproc
```

### Profiling Support
```c
// Profiling initialization
void _monstartup(void)
{
    extern char __text_start[], __etext[];
    _mcleanup();
    monstartup(__text_start, __etext);
}
```

## Standards Compliance

### System V ABI Compliance
- Follows RISC-V ELF ABI specification
- Implements required symbol visibility
- Maintains calling convention compatibility
- Supports position-independent code

### POSIX Requirements
- Implements required startup behavior
- Supports process initialization
- Handles environment variables correctly
- Provides proper exit handling

## Security Considerations

### Stack Protection
```assembly
# Stack canary initialization
la t0, __stack_chk_guard
ld t1, 0(t0)
sd t1, STACK_CANARY_OFFSET(sp)
```

### Address Space Layout
- Randomization support
- Stack protection
- GOT/PLT security
- Safe exception handling

## Testing and Validation

### Startup Testing
```c
// Test program for startup validation
int main(int argc, char *argv[], char *envp[])
{
    printf("argc: %d\n", argc);
    printf("argv[0]: %s\n", argv[0]);
    printf("TLS working: %s\n", tls_test() ? "yes" : "no");
    return 0;
}
```

### Constructor Testing
```c
// Test constructor execution
__attribute__((constructor))
void test_constructor(void)
{
    printf("Constructor executed\n");
}

__attribute__((destructor))
void test_destructor(void)
{
    printf("Destructor executed\n");
}
```

## Common Issues and Solutions

### Stack Misalignment
```assembly
# Ensure proper alignment
andi sp, sp, -16
addi sp, sp, -FRAME_SIZE
```

### Missing Symbols
```makefile
# Ensure all required symbols are provided
PROVIDE(__global_pointer$ = .);
PROVIDE(_gp = .);
```

### TLS Initialization Failures
```c
// Robust TLS initialization
if (_tls_init() != 0) {
    _exit(1);  // Fatal error
}
```

The RISC-V 64-bit C startup code provides the essential runtime foundation for MINIX 3 applications, ensuring proper initialization, efficient execution, and reliable termination while leveraging RISC-V architecture features for optimal performance.