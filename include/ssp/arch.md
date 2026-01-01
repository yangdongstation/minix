# MINIX Stack-Smashing Protection (SSP) Headers Architecture

## Directory Overview

The `/include/ssp/` directory contains the Stack-Smashing Protection (SSP) implementation, also known as ProPolice or FORTIFY_SOURCE. This security feature provides runtime protection against buffer overflow attacks by detecting stack corruption before it can be exploited for malicious code execution.

## Security Architecture Foundation

### SSP Protection Mechanisms
- **Stack Canaries**: Special guard values placed between stack frames
- **Buffer Overflow Detection**: Runtime bounds checking for string operations
- **Function Prologue/Epilogue**: Enhanced stack frame setup with protection
- **Safe Function Wrappers**: Fortified versions of vulnerable library functions
- **Compile-time Analysis**: Static detection of potential overflow conditions

### Threat Model
SSP protects against:
- Stack-based buffer overflows
- Return address corruption
- Function pointer overwrites
- Local variable corruption
- Stack pivot attacks
- Return-oriented programming (ROP) chains

## Header Files and Protection Levels

### Core SSP Framework

#### `ssp.h` - Master SSP Definitions
**Purpose**: Primary header defining SSP protection levels and core mechanisms

**Protection Levels**:
```c
// SSP protection levels based on _FORTIFY_SOURCE
#define __SSP_FORTIFY_LEVEL 0    // No protection
#define __SSP_FORTIFY_LEVEL 1    // Basic protection
#define __SSP_FORTIFY_LEVEL 2    // Enhanced protection

// Level 0: No protection (disabled or C++ code)
#if __SSP_FORTIFY_LEVEL == 0
#define __ssp_real_(fun)    fun

// Level 1: Basic runtime checks
#elif __SSP_FORTIFY_LEVEL == 1
#define __ssp_real_(fun)    __ssp_real_ ## fun

// Level 2: Enhanced protection with object size tracking
#elif __SSP_FORTIFY_LEVEL == 2
#define __ssp_real_(fun)    __ssp_real_ ## fun
#endif
```

**Core SSP Functions**:
```c
// Stack protection failure handlers
void __stack_chk_fail(void) __dead;    // Stack canary violation
void __chk_fail(void) __dead;          // Fortify check failure

// Buffer overflow detection macros
#define __ssp_bos(ptr) __builtin_object_size(ptr, __SSP_FORTIFY_LEVEL > 1)
#define __ssp_bos0(ptr) __builtin_object_size(ptr, 0)

// Runtime validation
#define __ssp_check(buf, len, bos) \
    if (bos(buf) != (size_t)-1 && len > bos(buf)) \
        __chk_fail()
```

**Inline Protection Macros**:
```c
// Function redirection for fortified versions
#define __ssp_redirect_raw(rtype, fun, symbol, args, call, cond, bos) \
rtype __ssp_real_(fun) args __RENAME(symbol); \
__ssp_inline rtype fun args __RENAME(__ssp_protected_ ## fun); \
__ssp_inline rtype fun args { \
    if (cond) \
        __ssp_check(__buf, __len, bos); \
    return __ssp_real_(fun) call; \
}

// Standard redirection macros
#define __ssp_redirect(rtype, fun, args, call) \
    __ssp_redirect_raw(rtype, fun, fun, args, call, 1, __ssp_bos)
#define __ssp_redirect0(rtype, fun, args, call) \
    __ssp_redirect_raw(rtype, fun, fun, args, call, 1, __ssp_bos0)
```

**Memory Overlap Detection**:
```c
// Detect overlapping memory regions
#define __ssp_overlap(a, b, l) \
    (((a) <= (b) && (b) < (a) + (l)) || ((b) <= (a) && (a) < (b) + (l)))
```

### String Operation Protection

#### `string.h` - Fortified String Functions
**Purpose**: Provides SSP-protected versions of standard string manipulation functions

**Protected String Functions**:
```c
// Memory operations with bounds checking
void *memcpy(void *dst, const void *src, size_t len)
    __ssp_redirect(void *, memcpy, (dst, src, len), (dst, src, len))

void *memmove(void *dst, const void *src, size_t len)
    __ssp_redirect(void *, memmove, (dst, src, len), (dst, src, len))

void *memset(void *b, int c, size_t len)
    __ssp_redirect(void *, memset, (b, c, len), (b, c, len))

void *bcopy(const void *src, void *dst, size_t len)
    __ssp_redirect(void *, bcopy, (src, dst, len), (src, dst, len))

void bzero(void *b, size_t len)
    __ssp_redirect(void, bzero, (b, len), (b, len))
```

**String Copy Protection**:
```c
// String copying with overflow protection
char *strcpy(char *dst, const char *src)
    __ssp_redirect0(char *, strcpy, (dst, src), (dst, src))

char *strncpy(char *dst, const char *src, size_t len)
    __ssp_redirect(char *, strncpy, (dst, src, len), (dst, src, len))

char *stpcpy(char *dst, const char *src)
    __ssp_redirect0(char *, stpcpy, (dst, src), (dst, src))
```

**String Concatenation Protection**:
```c
// String concatenation with bounds checking
char *strcat(char *s, const char *append)
    __ssp_redirect0(char *, strcat, (s, append), (s, append))

char *strncat(char *s, const char *append, size_t len)
    __ssp_redirect(char *, strncat, (s, append, len), (s, append, len))
```

**String Length Protection**:
```c
// Safe string length operations
size_t strlen(const char *str)
    __ssp_redirect(size_t, strlen, (str), (str))

size_t strnlen(const char *str, size_t maxlen)
    __ssp_redirect(size_t, strnlen, (str, maxlen), (str, maxlen))
```

#### `strings.h` - BSD String Function Protection
**Purpose**: SSP protection for BSD-style string functions

**Protected BSD Functions**:
```c
// BSD string operations
int bcmp(const void *b1, const void *b2, size_t len)
    __ssp_redirect(int, bcmp, (b1, b2, len), (b1, b2, len))

char *index(const char *s, int c)
    __ssp_redirect(char *, index, (s, c), (s, c))

char *rindex(const char *s, int c)
    __ssp_redirect(char *, rindex, (s, c), (s, c))
```

### I/O Operation Protection

#### `stdio.h` - Fortified Standard I/O Functions
**Purpose**: Provides SSP-protected versions of standard I/O operations

**Protected I/O Functions**:
```c
// Formatted input with bounds checking
char *gets(char *buf)
    __ssp_redirect0(char *, gets, (buf), (buf))

char *fgets(char *buf, int n, FILE *fp)
    __ssp_redirect(char *, fgets, (buf, n, fp), (buf, n, fp))

// Formatted output protection
int sprintf(char *buf, const char *fmt, ...)
    __ssp_redirect(int, sprintf, (buf, fmt, __VA_ARGS__), (buf, fmt, __VA_ARGS__))

int vsprintf(char *buf, const char *fmt, va_list ap)
    __ssp_redirect(int, vsprintf, (buf, fmt, ap), (buf, fmt, ap))

int snprintf(char *buf, size_t len, const char *fmt, ...)
    __ssp_redirect(int, snprintf, (buf, len, fmt, __VA_ARGS__), (buf, len, fmt, __VA_ARGS__))

int vsnprintf(char *buf, size_t len, const char *fmt, va_list ap)
    __ssp_redirect(int, vsnprintf, (buf, len, fmt, ap), (buf, len, fmt, ap))
```

### System Call Protection

#### `unistd.h` - Fortified System Interface Functions
**Purpose**: SSP protection for system interface functions

**Protected System Functions**:
```c
// File operations with bounds checking
ssize_t read(int fd, void *buf, size_t nbyte)
    __ssp_redirect(ssize_t, read, (fd, buf, nbyte), (fd, buf, nbyte))

ssize_t write(int fd, const void *buf, size_t nbyte)
    __ssp_redirect(ssize_t, write, (fd, buf, nbyte), (fd, buf, nbyte))

ssize_t pread(int fd, void *buf, size_t nbyte, off_t offset)
    __ssp_redirect(ssize_t, pread, (fd, buf, nbyte, offset), (fd, buf, nbyte, offset))

ssize_t pwrite(int fd, const void *buf, size_t nbyte, off_t offset)
    __ssp_redirect(ssize_t, pwrite, (fd, buf, nbyte, offset), (fd, buf, nbyte, offset))
```

**Pathname Protection**:
```c
// Pathname operations
ssize_t readlink(const char *path, char *buf, size_t bufsize)
    __ssp_redirect(ssize_t, readlink, (path, buf, bufsize), (path, buf, bufsize))

int getcwd(char *buf, size_t size)
    __ssp_redirect(int, getcwd, (buf, size), (buf, size))
```

## RISC-V 64-bit Architecture Support

### Memory Alignment Requirements
```c
// Proper alignment for RISC-V 64-bit
struct ssp_canary {
    uintptr_t guard_value;      // 8-byte aligned on RV64
    uintptr_t saved_fp;         // Frame pointer
    uintptr_t saved_ra;         // Return address
} __attribute__((aligned(8)));

// Stack canary placement optimized for RV64
#define SSP_CANARY_GAP 16       // Optimal gap for RISC-V stack frames
```

### Performance Optimizations
- **Inline Functions**: All SSP checks implemented as static inline functions
- **Branch Prediction**: Optimized conditional checks for RV64 pipeline
- **Memory Access**: Efficient 64-bit memory operations for canary checks
- **Register Usage**: Minimal register pressure for SSP operations

### Atomic Operations
```c
// Atomic canary verification for concurrent access
static __inline __attribute__((__always_inline__)) void 
__ssp_verify_canary(uintptr_t *canary) {
    register uintptr_t guard asm("t0");  // Use temporary register
    guard = *canary;
    if (__builtin_expect(guard != __ssp_canary_value, 0))
        __stack_chk_fail();
}
```

## MINIX Microkernel Integration

### Message-Passing Protection
SSP integrates with MINIX's message-passing architecture:

```c
// Protected message buffer operations
#define SSP_MSG_COPY(dst, src, len) do { \
    if (__ssp_bos(dst) != (size_t)-1 && len > __ssp_bos(dst)) \
        __chk_fail(); \
    memcpy(dst, src, len); \
} while (0)

// Message validation in system calls
int sys_vircopy(endpoint_t src, vir_bytes src_addr,
                endpoint_t dst, vir_bytes dst_addr, vir_bytes len) {
    // Validate buffer sizes using SSP
    if (!__ssp_verify_buffers(dst, dst_addr, len))
        return EFAULT;
    // Perform copy operation
    return do_vircopy(src, src_addr, dst, dst_addr, len);
}
```

### Userspace Server Protection
- **VFS Server**: Protected pathname operations and buffer handling
- **PM Server**: Process management with SSP-protected operations
- **Network Server**: Safe packet buffer and protocol handling
- **Memory Server**: Protected memory allocation and copying

### Driver Framework Integration
```c
// Protected driver operations
ssize_t driver_read(struct device *dev, void *buf, size_t count, off_t offset) {
    // Validate buffer with SSP
    if (__ssp_bos(buf) != (size_t)-1 && count > __ssp_bos(buf))
        __chk_fail();
    // Perform device read
    return device_read(dev, buf, count, offset);
}
```

## Security Analysis

### Attack Vector Mitigation

#### Stack Buffer Overflows
- **Canary Values**: Random 64-bit guard values on RV64
- **Prologue/Epilogue**: Enhanced function entry/exit sequences
- **Safe Functions**: Fortified library functions prevent overflows
- **Return Address Protection**: RA register validation on RISC-V

#### Heap-based Attacks
- **Object Size Tracking**: `__builtin_object_size` for runtime checks
- **Heap Canaries**: Optional heap protection extensions
- **Use-after-Free**: Integration with memory debugging features
- **Double-Free**: Protected memory management operations

#### Format String Attacks
- **Format Validation**: Runtime format string validation
- **Argument Counting**: Automatic argument count verification
- **Type Safety**: Enhanced type checking for variadic functions

### Limitations and Considerations
- **Performance Overhead**: ~5-10% performance impact on protected functions
- **Memory Overhead**: Additional stack space for canaries and padding
- **Coverage**: Protection limited to fortified functions
- **Bypass Techniques**: Advanced attacks may circumvent SSP

## Configuration and Build Integration

### Compiler Flags
```bash
# Enable basic SSP protection
CFLAGS += -fstack-protector-strong

# Enable fortified source protection
CFLAGS += -D_FORTIFY_SOURCE=2 -O2

# RISC-V specific optimizations
CFLAGS += -march=rv64gc -mabi=lp64d
```

### Runtime Configuration
```c
// Runtime SSP configuration
extern int __ssp_global_enable;
extern int __ssp_verbose_failures;

// Per-process SSP settings
void ssp_enable_protection(int level) {
    if (level < 0 || level > 2) return;
    __ssp_global_enable = level;
}
```

### Debugging Support
```c
// SSP failure debugging
#define SSP_DEBUG_CANARY  0x01    // Debug canary violations
#define SSP_DEBUG_BOUNDS  0x02    // Debug bounds violations
#define SSP_DEBUG_VERBOSE 0x04    // Verbose debugging output

extern unsigned int __ssp_debug_flags;
```

## Testing and Validation

### Security Testing
- **Canary Validation**: Verify canary placement and checking
- **Bounds Checking**: Test buffer overflow detection
- **Function Redirection**: Validate fortified function calls
- **Memory Corruption**: Simulate attack scenarios

### Performance Testing
- **Benchmark Overhead**: Measure SSP performance impact
- **Memory Usage**: Analyze additional memory requirements
- **Scalability**: Test under high-load conditions
- **RISC-V Optimization**: Verify RV64-specific optimizations

### Compatibility Testing
- **ABI Compatibility**: Ensure SSP doesn't break binary compatibility
- **Library Integration**: Test with third-party libraries
- **System Call Interface**: Validate kernel interface protection
- **Cross-Architecture**: Test portability across architectures

## Development Guidelines

### Best Practices
1. **Enable Protection**: Use `-fstack-protector-strong` and `-D_FORTIFY_SOURCE=2`
2. **Validate Input**: Always validate external input before processing
3. **Safe Functions**: Prefer fortified versions of library functions
4. **Error Handling**: Implement proper SSP failure handling
5. **Testing**: Include security testing in development cycle

### Code Guidelines
```c
// Safe string handling
char buffer[256];
if (sscanf(input, "%255s", buffer) != 1) {
    // Handle error
    return -1;
}

// Protected memory operations
void safe_copy(void *dst, const void *src, size_t len) {
    // SSP automatically checks bounds
    memcpy(dst, src, len);
}

// Custom fortified functions
__ssp_inline void *my_memcpy(void *dst, const void *src, size_t len) {
    __ssp_check(dst, len, __ssp_bos);
    return __ssp_real_memcpy(dst, src, len);
}
```

This comprehensive SSP implementation provides robust protection against buffer overflow attacks while maintaining compatibility with existing code and optimizing performance for the RISC-V 64-bit architecture.