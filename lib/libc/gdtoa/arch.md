# Floating-Point Conversion Library Architecture

## Overview

The `lib/libc/gdtoa/` directory contains the `gdtoa` (General Decimal to Arithmetic) library, a comprehensive floating-point conversion library that provides accurate and efficient conversion between decimal strings and binary floating-point formats. This library is critical for standard I/O operations, mathematical computations, and data serialization in MINIX 3.

## Purpose and Responsibilities

### Core Conversion Functions
- **String to Floating-Point**: Parse decimal strings to IEEE 754 formats
- **Floating-Point to String**: Convert binary formats to decimal representation
- **Multiple Precision Support**: Extended precision arithmetic operations
- **Rounding Control**: Configurable rounding modes per IEEE 754
- **Exception Handling**: Proper handling of overflow, underflow, and invalid operations
- **Locale Support**: Internationalization for decimal point characters

### Supported Data Types
- **Single Precision**: `float` (32-bit IEEE 754)
- **Double Precision**: `double` (64-bit IEEE 754)
- **Extended Precision**: `long double` (80-bit or 128-bit)
- **Quadruple Precision**: `__float128` (128-bit IEEE 754)
- **Integer Types**: Conversion to/from integer formats

## Directory Structure

```
lib/libc/gdtoa/
├── arithchk.c          # Arithmetic consistency checking
├── dmisc.c             # Double-precision miscellaneous functions
├── dtoa.c              # Core double-to-ASCII conversion
├── g_ddfmt.c           # Double-double format support
├── g_dfmt.c            # Default format conversion
├── gdtoa.c             # Main gdtoa implementation
├── gdtoa.h             # Public API header
├── gdtoaimp.h          # Internal implementation header
├── gdtoa_fltrnds.h     # Floating-point rounding definitions
├── gdtoa_locks.c       # Thread synchronization
├── gethex.c            # Hexadecimal floating-point support
├── g_ffmt.c            # Float format conversion
├── g__fmt.c            # Generic format conversion
├── gmisc.c             # General miscellaneous functions
├── g_Qfmt.c            # Quadruple precision format
├── g_xfmt.c            # Extended precision format
├── g_xLfmt.c           # Extended long precision format
├── hd_init.c           # Hexadecimal initialization
├── hdtoa.c             # Hexadecimal double-to-ASCII
├── hexnan.c            # Hexadecimal NaN handling
├── ldtoa.c             # Long double to ASCII
├── misc.c              # Miscellaneous utility functions
├── qnan.c              # Quiet NaN generation
├── smisc.c             # Single-precision miscellaneous
├── strtod*.c           # String-to-double variants
├── strtof*.c           # String-to-float variants
├── strtor*.c           # Rounding-specific variants
├── strtop*.c           # Parsing variants
├── sum.c               # Summation algorithms
├── test/               # Test suite
├── ulp.c               # Unit in the last place calculations
└── xsum0.out           # Expected test output
```

## Core Algorithm Architecture

### Multi-Precision Arithmetic
The library uses a multi-precision arithmetic engine for accurate conversions:

```c
// Big integer structure for extended precision
typedef struct Bigint {
    struct Bigint *next;    // Linked list for memory management
    int k;                  // Size in 32-bit words
    int maxwds;             // Maximum words allocated
    int sign;               // Sign bit
    unsigned long x[1];     // Variable-length array
} Bigint;

// Multiplication algorithm
static Bigint *mult(Bigint *a, Bigint *b)
{
    Bigint *c;
    int carry;
    unsigned long *x, *xa, *xae, *xb, *xbe;
    unsigned long y;
    
    // Allocate result with sufficient size
    c = Balloc(a->k + b->k);
    
    // Perform multi-precision multiplication
    for (xa = a->x, xae = xa + a->wds; xa < xae; xa++) {
        carry = 0;
        y = *xa;
        for (xb = b->x, xbe = xb + b->wds, x = xa + xb - b->x; xb < xbe; xb++, x++) {
            unsigned long z = *x + y * *xb + carry;
            carry = z >> 32;
            *x = z & 0xFFFFFFFF;
        }
        if (carry) {
            *x = carry;
        }
    }
    
    return c;
}
```

### Accurate Conversion Algorithm
```c
// Core conversion algorithm
double strtod_gdtoa(const char *s00, char **se, int *error)
{
    int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, dsign, e, e1;
    int esign, i, j, k, nd, nd0, nf, nz, nz0, sign;
    const char *s, *s0, *s1;
    double aadj, aadj1, adj, rv, rv0;
    long L;
    unsigned long y, z;
    Bigint *bb, *bb1, *bd, *bd0, *bs, *delta;
    
    // Parse sign and skip whitespace
    sign = 1;
    while (*s00 == ' ' || *s00 == '\t') s00++;
    if (*s00 == '-') {
        sign = -1;
        s00++;
    } else if (*s00 == '+') {
        s00++;
    }
    
    // Parse mantissa
    s0 = s = s00;
    nd = nf = 0;
    while ((c = *s) >= '0' && c <= '9') {
        nd++;
        if (c != '0') nf = nd;
        s++;
    }
    
    // Parse exponent if present
    if (*s == 'e' || *s == 'E') {
        s++;
        esign = 1;
        if (*s == '-') {
            esign = -1;
            s++;
        } else if (*s == '+') {
            s++;
        }
        e = 0;
        while ((c = *s) >= '0' && c <= '9') {
            e = e * 10 + (c - '0');
            s++;
        }
        e *= esign;
    }
    
    // Convert to binary using multi-precision arithmetic
    // ... complex conversion logic
    
    return sign * rv;
}
```

## RISC-V 64-bit Specific Optimizations

### IEEE 754 Format Handling
Optimized for RISC-V 64-bit IEEE 754 implementation:

```c
// RISC-V 64-bit double precision extraction
#define EXTRACT_DOUBLE_BITS(d, sign, exp, mant) do { \
    union { double d; uint64_t u; } u; \
    u.d = (d); \
    (sign) = (u.u >> 63) & 1; \
    (exp) = (u.u >> 52) & 0x7FF; \
    (mant) = u.u & 0xFFFFFFFFFFFFFULL; \
} while (0)

// RISC-V optimized NaN and infinity detection
static inline int is_riscv_nan(double d)
{
    uint64_t bits;
    union { double d; uint64_t u; } u;
    u.d = d;
    bits = u.u;
    
    // Check exponent all 1s and mantissa non-zero
    return ((bits >> 52) & 0x7FF) == 0x7FF && (bits & 0xFFFFFFFFFFFFFULL) != 0;
}
```

### Multi-Precision Word Size Optimization
Optimized for RISC-V 64-bit word operations:

```c
// RISC-V 64-bit optimized multi-precision operations
#ifdef __riscv64
    // Use 64-bit operations for better performance
    typedef uint64_t mp_word;
    #define MP_WORD_SIZE 64
    #define MP_WORD_BITS 6  // log2(64)
#else
    // Fallback to 32-bit operations
    typedef uint32_t mp_word;
    #define MP_WORD_SIZE 32
    #define MP_WORD_BITS 5  // log2(32)
#endif

// Fast multi-precision addition for RISC-V 64-bit
static void mp_add_64(mp_word *c, const mp_word *a, const mp_word *b, int n)
{
    uint64_t carry = 0;
    for (int i = 0; i < n; i++) {
        uint64_t sum = (uint64_t)a[i] + b[i] + carry;
        c[i] = sum & 0xFFFFFFFFFFFFFFFFULL;
        carry = sum >> 64;
    }
}
```

### Atomic Operations for Thread Safety
```c
// Thread-safe reference counting for shared state
static volatile int gdtoa_init_count = 0;

void gdtoa_thread_init(void)
{
    __atomic_add_fetch(&gdtoa_init_count, 1, __ATOMIC_SEQ_CST);
    
    if (__atomic_load_n(&gdtoa_init_count, __ATOMIC_SEQ_CST) == 1) {
        // First thread initializes shared state
        _gdtoa_global_init();
    }
}

void gdtoa_thread_fini(void)
{
    if (__atomic_sub_fetch(&gdtoa_init_count, 1, __ATOMIC_SEQ_CST) == 0) {
        // Last thread cleans up shared state
        _gdtoa_global_cleanup();
    }
}
```

## Thread Safety Architecture

### Reentrant Design
```c
// Thread-local storage for conversion state
typedef struct {
    Bigint *freelist[32];      // Free list for memory management
    int rounding_mode;         // Current rounding mode
    int exceptions;            // Exception flags
    char *convbuf;             // Conversion buffer
    size_t convbuf_size;       // Buffer size
} gdtoa_thread_t;

static __thread gdtoa_thread_t *gdtoa_tls = NULL;

// Per-thread initialization
gdtoa_thread_t *gdtoa_get_thread_state(void)
{
    if (gdtoa_tls == NULL) {
        gdtoa_tls = calloc(1, sizeof(gdtoa_thread_t));
        gdtoa_tls->convbuf_size = 1024;
        gdtoa_tls->convbuf = malloc(gdtoa_tls->convbuf_size);
    }
    return gdtoa_tls;
}
```

### Lock-Free Algorithms
```c
// Lock-free reference counting for shared tables
static volatile uint32_t table_refcount[256];

void acquire_table_ref(int index)
{
    uint32_t old, new;
    do {
        old = __atomic_load_n(&table_refcount[index], __ATOMIC_ACQUIRE);
        new = old + 1;
    } while (!__atomic_compare_exchange_n(&table_refcount[index], &old, new,
                                          0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE));
}

void release_table_ref(int index)
{
    __atomic_sub_fetch(&table_refcount[index], 1, __ATOMIC_RELEASE);
}
```

## Memory Management

### Efficient Allocation Strategy
```c
// Memory pool for frequent allocations
typedef struct Mem_node {
    struct Mem_node *next;
    size_t size;
} Mem_node;

static Mem_node *free_list[32];  // Power-of-2 size classes

// Fast allocation from pool
void *gdtoa_malloc(size_t size)
{
    int k = 0;
    size_t sz = sizeof(Bigint) + size * sizeof(unsigned long);
    
    // Find appropriate size class
    while ((1 << k) < sz) k++;
    
    if (free_list[k]) {
        // Reuse from free list
        Mem_node *node = free_list[k];
        free_list[k] = node->next;
        return node;
    }
    
    // Allocate new block
    return malloc(1 << k);
}

// Fast deallocation to pool
void gdtoa_free(void *ptr, size_t size)
{
    int k = 0;
    while ((1 << k) < size) k++;
    
    Mem_node *node = ptr;
    node->next = free_list[k];
    free_list[k] = node;
}
```

### Garbage Collection
```c
// Reference counting for Bigint structures
struct Bigint {
    int refcount;
    struct Bigint *next;
    int k, maxwds, sign;
    unsigned long x[1];
};

// Reference counting operations
static inline Bigint *Bcopy(Bigint *b)
{
    __atomic_add_fetch(&b->refcount, 1, __ATOMIC_SEQ_CST);
    return b;
}

static inline void Bfree(Bigint *b)
{
    if (__atomic_sub_fetch(&b->refcount, 1, __ATOMIC_SEQ_CST) == 0) {
        b->next = freelist[b->k];
        freelist[b->k] = b;
    }
}
```

## Performance Optimizations

### Fast Path for Common Cases
```c
// Fast path for simple integer values
double fast_strtod_int(const char *s, char **endptr)
{
    unsigned long long val = 0;
    int sign = 1;
    const char *p = s;
    
    // Handle sign
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    
    // Parse digits
    while (*p >= '0' && *p <= '9') {
        int digit = *p - '0';
        if (val > (ULLONG_MAX - digit) / 10) {
            // Overflow, fall back to slow path
            return strtod_gdtoa(s, endptr, NULL);
        }
        val = val * 10 + digit;
        p++;
    }
    
    // No decimal point or exponent - fast return
    if (*p != '.' && *p != 'e' && *p != 'E') {
        *endptr = (char *)p;
        return sign * (double)val;
    }
    
    // Fall back to full conversion
    return strtod_gdtoa(s, endptr, NULL);
}
```

### Table-Driven Algorithms
```c
// Precomputed powers of 10 for fast conversion
static const double powers_of_10[] = {
    1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
    1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
    1e20, 1e21, 1e22
};

// Fast exponentiation using precomputed tables
double pow10_fast(int n)
{
    if (n >= 0 && n < 23) {
        return powers_of_10[n];
    }
    
    // Handle larger exponents
    if (n < 0) {
        return 1.0 / pow10_fast(-n);
    }
    
    // Decompose into smaller exponents
    double result = 1.0;
    while (n > 22) {
        result *= powers_of_10[22];
        n -= 22;
    }
    return result * powers_of_10[n];
}
```

## Error Handling and Standards Compliance

### IEEE 754 Exception Handling
```c
// Exception flag management
void set_exception_flag(int flag)
{
    fenv_t env;
    fegetenv(&env);
    
    switch (flag) {
    case FE_OVERFLOW:
        env.__exception |= FE_OVERFLOW;
        break;
    case FE_UNDERFLOW:
        env.__exception |= FE_UNDERFLOW;
        break;
    case FE_INVALID:
        env.__exception |= FE_INVALID;
        break;
    case FE_INEXACT:
        env.__exception |= FE_INEXACT;
        break;
    }
    
    fesetenv(&env);
}

// Check for exceptions after conversion
int check_conversion_exceptions(double result, int *error)
{
    if (isinf(result)) {
        *error = ERANGE;
        set_exception_flag(FE_OVERFLOW);
        return -1;
    }
    
    if (result != 0.0 && fabs(result) < DBL_MIN) {
        *error = ERANGE;
        set_exception_flag(FE_UNDERFLOW);
        return -1;
    }
    
    return 0;
}
```

### POSIX Compliance
```c
// errno setting for POSIX compliance
double strtod_posix(const char *nptr, char **endptr)
{
    errno = 0;
    
    // Skip leading whitespace
    while (isspace(*nptr)) nptr++;
    
    // Check for valid number format
    if (!isdigit(*nptr) && *nptr != '.' && *nptr != '+' && *nptr != '-') {
        if (endptr) *endptr = (char *)nptr;
        return 0.0;
    }
    
    // Perform conversion
    double result = strtod_gdtoa(nptr, endptr, &errno);
    
    // Check for conversion errors
    if (endptr && *endptr == nptr) {
        errno = EINVAL;
        return 0.0;
    }
    
    return result;
}
```

## Build Configuration

### Architecture-Specific Flags
```makefile
# RISC-V 64-bit optimizations
CFLAGS += -march=rv64gc -mabi=lp64d
CFLAGS += -frounding-math -fsignaling-nans
CFLAGS += -fno-builtin-strtod -fno-builtin-snprintf

# Enable optimizations for 64-bit word operations
CFLAGS += -DUSE_64BIT_WORDS -DUSE_FAST_PATH

# Thread safety support
CFLAGS += -DTHREAD_SAFE -DUSE_ATOMIC_OPS
```

### Source File Organization
```makefile
# Core conversion sources
GDTOA_SRCS += \
    gdtoa.c \
    dtoa.c \
    misc.c \
    gmisc.c \
    smisc.c \
    dmisc.c

# String-to-float conversion sources
GDTOA_SRCS += \
    strtod.c \
    strtodg.c \
    strtof.c \
    strtold.c

# Hexadecimal conversion support
GDTOA_SRCS += \
    hdtoa.c \
    gethex.c \
    hexnan.c

# Format-specific sources
GDTOA_SRCS += \
    g_ffmt.c \
    g_dfmt.c \
    g_xfmt.c \
    g_Qfmt.c
```

## Testing and Validation

### Accuracy Testing
```c
// Test conversion accuracy
void test_conversion_accuracy(void)
{
    struct test_case {
        const char *input;
        double expected;
        int line;
    } tests[] = {
        {"1.0", 1.0, __LINE__},
        {"1.5", 1.5, __LINE__},
        {"1e10", 1e10, __LINE__},
        {"1e-10", 1e-10, __LINE__},
        {"3.1415926535897932", M_PI, __LINE__},
    };
    
    for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        char *end;
        double result = strtod(tests[i].input, &end);
        
        if (result != tests[i].expected) {
            printf("Test failed at line %d: %s -> %a (expected %a)\n",
                   tests[i].line, tests[i].input, result, tests[i].expected);
        }
    }
}
```

### Performance Benchmarking
```c
// Benchmark conversion performance
void benchmark_conversions(void)
{
    const int iterations = 1000000;
    const char *test_strings[] = {
        "1.0", "3.14159", "2.71828", "1.41421", "1.73205"
    };
    
    clock_t start = clock();
    
    for (int i = 0; i < iterations; i++) {
        char *end;
        for (int j = 0; j < 5; j++) {
            strtod(test_strings[j], &end);
        }
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("Converted %d strings in %.3f seconds (%.0f conversions/sec)\n",
           iterations * 5, elapsed, (iterations * 5) / elapsed);
}
```

The gdtoa library provides the mathematical foundation for accurate floating-point conversions in MINIX 3, with RISC-V 64-bit optimizations ensuring both precision and performance for critical numerical operations.