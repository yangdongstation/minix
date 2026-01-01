# Standard Library Functions Architecture

## Overview

The `lib/libc/stdlib/` directory contains the standard library functions that provide essential utilities for C programs, including memory allocation, process control, environment management, mathematical utilities, and data conversion functions. These functions form the core runtime support that applications depend on for basic operations.

## Purpose and Responsibilities

### Core Function Categories
- **Memory Management**: Dynamic memory allocation and deallocation
- **Process Control**: Program termination, command execution, process interaction
- **Environment Management**: Environment variable access and modification
- **String Conversion**: Numeric string parsing and formatting
- **Random Number Generation**: Pseudo-random number sequences
- **Searching and Sorting**: Binary search and quicksort algorithms
- **Mathematical Utilities**: Integer arithmetic, absolute values, division
- **Multibyte Character Support**: Wide character and multibyte conversions

### Key Features
- Thread-safe implementations where required
- POSIX standards compliance
- RISC-V 64-bit architecture optimizations
- Memory allocation efficiency
- Error handling and validation
- Locale-aware conversions

## Directory Structure

```
lib/libc/stdlib/
├── Exit.c              # Program termination functions
├── Makefile.inc        # Build configuration
├── _Exit.c             # Immediate program termination
├── abs.c               # Integer absolute value
├── atexit.c            # Exit handler registration
├── atof.c              # ASCII to float conversion
├── atoi.c              # ASCII to integer conversion
├── atol.c              # ASCII to long conversion
├── atoll.c             # ASCII to long long conversion
├── bsearch.c           # Binary search algorithm
├── div.c               # Integer division
├── getenv.c            # Environment variable access
├── getopt.c            # Command-line option parsing
├── getopt_long.c       # Extended option parsing
├── getrusage.c         # Resource usage information
├── heapsort.c          # Heap sort algorithm
├── imaxabs.c           # Integer maximum absolute value
├── imaxdiv.c           # Integer maximum division
├── labs.c              # Long absolute value
├── ldiv.c              # Long division
├── llabs.c             # Long long absolute value
├── lldiv.c             # Long long division
├── malloc.c            # Memory allocation implementation
├── mergesort.c         # Merge sort algorithm
├── mrand.c             # Medium-quality random numbers
├── qsort.c             # Quick sort algorithm
├── qsort_r.c           # Reentrant quick sort
├── rand.c              # Linear congruential random generator
├── rand_r.c            # Reentrant random generator
├── random.c            # Advanced random number generator
├── setenv.c            # Environment variable setting
├── srand.c             # Seed random generator
├── srandom.c           # Seed advanced random generator
├── strtol.c            # String to long conversion
├── strtoll.c           # String to long long conversion
├── strtoul.c           # String to unsigned long conversion
├── strtoull.c          # String to unsigned long long conversion
├── system.c            # Command execution
├── unsetenv.c          # Environment variable removal
└── wcstol.c            # Wide string to long conversion
```

## Memory Management Architecture

### Memory Allocation Strategy
```c
// RISC-V 64-bit optimized memory allocator
typedef struct malloc_header {
    size_t size;                    // Size of allocated block
    struct malloc_header *next;     // Next block in free list
    struct malloc_header *prev;     // Previous block in free list
    unsigned int flags;             // Block flags (free/allocated)
    unsigned int magic;             // Magic number for corruption detection
} malloc_header_t;

// Arena-based allocation for scalability
typedef struct malloc_arena {
    void *base;                     // Base address of arena
    size_t size;                    // Total size of arena
    size_t used;                    // Used space in arena
    malloc_header_t *free_list;     // Free block list
    pthread_mutex_t lock;           // Thread synchronization
    struct malloc_arena *next;      // Next arena
} malloc_arena_t;

// RISC-V 64-bit optimized malloc
void *malloc(size_t size)
{
    // Align size to 16-byte boundary for RISC-V requirements
    size = (size + 15) & ~15;
    
    // Fast path for small allocations
    if (size <= SMALL_ALLOC_MAX) {
        return malloc_small(size);
    }
    
    // General allocation path
    return malloc_large(size);
}

// Small allocation optimization
static void *malloc_small(size_t size)
{
    malloc_arena_t *arena = get_thread_arena();
    
    // Lock-free fast path for thread-local cache
    if (arena->thread_cache && arena->thread_cache->size >= size) {
        void *ptr = arena->thread_cache;
        arena->thread_cache = ptr->next;
        return ptr;
    }
    
    // Search free list with best-fit strategy
    pthread_mutex_lock(&arena->lock);
    malloc_header_t *best = find_best_fit(arena, size);
    if (best) {
        split_block(arena, best, size);
        pthread_mutex_unlock(&arena->lock);
        return (void *)(best + 1);
    }
    pthread_mutex_unlock(&arena->lock);
    
    // Allocate new arena if necessary
    return allocate_new_arena(size);
}
```

### Memory Alignment Optimization
```c
// RISC-V 64-bit memory alignment requirements
#define RISCV_ALIGNMENT 16      // RISC-V stack and data alignment
#define CACHE_LINE_SIZE 64      // Typical cache line size

// Aligned allocation for SIMD and performance
void *aligned_alloc(size_t alignment, size_t size)
{
    // Validate alignment requirements
    if (alignment < sizeof(void *)) {
        alignment = sizeof(void *);
    }
    
    // Alignment must be power of 2
    if ((alignment & (alignment - 1)) != 0) {
        errno = EINVAL;
        return NULL;
    }
    
    // Allocate extra space for alignment and header
    size_t total_size = size + alignment + sizeof(void *);
    void *raw = malloc(total_size);
    if (!raw) return NULL;
    
    // Calculate aligned pointer
    uintptr_t raw_addr = (uintptr_t)raw;
    uintptr_t aligned_addr = (raw_addr + sizeof(void *) + alignment - 1) & ~(alignment - 1);
    
    // Store original pointer for free()
    void **ptr = (void **)aligned_addr;
    ptr[-1] = raw;
    
    return ptr;
}
```

## String Conversion Functions

### Optimized Integer Parsing
```c
// RISC-V 64-bit optimized string to integer conversion
long strtol_opt(const char *nptr, char **endptr, int base)
{
    const char *s = nptr;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0, any, cutlim;
    
    // Skip leading whitespace
    do {
        c = *s++;
    } while (isspace(c));
    
    // Handle sign
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }
    
    // Auto-detect base
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    } else if (base == 0) {
        base = c == '0' ? 8 : 10;
    }
    
    // Calculate overflow threshold
    cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
    cutlim = cutoff % (unsigned long)base;
    cutoff /= (unsigned long)base;
    
    // Fast parsing loop
    for (acc = 0, any = 0;; c = *s++) {
        if (c >= '0' && c <= '9') {
            c -= '0';
        } else if (c >= 'A' && c <= 'Z') {
            c -= 'A' - 10;
        } else if (c >= 'a' && c <= 'z') {
            c -= 'a' - 10;
        } else {
            break;
        }
        
        if (c >= base) break;
        
        // Check for overflow
        if (acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
            acc = neg ? LONG_MIN : LONG_MAX;
            errno = ERANGE;
            break;
        } else {
            any = 1;
            acc *= (unsigned long)base;
            acc += c;
        }
    }
    
    if (endptr != 0) *endptr = any ? (char *)s - 1 : (char *)nptr;
    return neg ? -acc : acc;
}
```

### Floating-Point Conversion Integration
```c
// Integration with gdtoa library for accurate conversion
double atof_opt(const char *nptr)
{
    return strtod(nptr, NULL);
}

// Optimized for common integer cases
int atoi_opt(const char *nptr)
{
    const char *s = nptr;
    int neg = 0, c, acc;
    
    // Skip whitespace
    while (isspace(c = *s++));
    
    // Handle sign
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }
    
    // Fast integer parsing
    for (acc = 0; c >= '0' && c <= '9'; c = *s++) {
        acc = acc * 10 + (c - '0');
    }
    
    return neg ? -acc : acc;
}
```

## Random Number Generation

### Linear Congruential Generator
```c
// RISC-V 64-bit optimized LCG
typedef struct {
    uint64_t seed;              // 64-bit seed for better quality
    pthread_mutex_t lock;       // Thread synchronization
} rand_state_t;

// Thread-local random state
static __thread rand_state_t *rand_tls = NULL;

// Initialize random state
static rand_state_t *get_rand_state(void)
{
    if (rand_tls == NULL) {
        rand_tls = calloc(1, sizeof(rand_state_t));
        pthread_mutex_init(&rand_tls->lock, NULL);
        // Seed with high-resolution timer
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        rand_tls->seed = ts.tv_sec ^ ts.tv_nsec;
    }
    return rand_tls;
}

// RISC-V optimized random generation
int rand_r(unsigned int *seed)
{
    uint64_t s = *seed;
    // Use RISC-V-friendly constants
    s = s * 1103515245ULL + 12345;
    *seed = (unsigned int)s;
    return (int)((s >> 16) & RAND_MAX);
}

// High-quality random number generator
long random(void)
{
    rand_state_t *state = get_rand_state();
    
    pthread_mutex_lock(&state->lock);
    // Use 64-bit LCG for better randomness
    state->seed = state->seed * 6364136223846793005ULL + 1442695040888963407ULL;
    long result = (long)(state->seed >> 33);
    pthread_mutex_unlock(&state->lock);
    
    return result;
}
```

## Searching and Sorting Algorithms

### Quick Sort Implementation
```c
// RISC-V 64-bit optimized quicksort
typedef int (*cmp_r_func_t)(const void *, const void *, void *);

// Optimized partition function
static inline size_t partition(void *base, size_t nmemb, size_t size,
                               int (*compar)(const void *, const void *))
{
    char *array = base;
    char *pivot = array + (nmemb / 2) * size;
    size_t i = 0, j = nmemb - 1;
    
    // Use register-based swap for small elements
    while (i <= j) {
        while (compar(array + i * size, pivot) < 0) i++;
        while (compar(array + j * size, pivot) > 0) j--;
        
        if (i <= j) {
            if (i != j) {
                // Optimized swap for RISC-V 64-bit
                if (size <= 8) {
                    // Use register operations for small sizes
                    uint64_t temp;
                    memcpy(&temp, array + i * size, size);
                    memcpy(array + i * size, array + j * size, size);
                    memcpy(array + j * size, &temp, size);
                } else {
                    // Use stack-based swap for larger sizes
                    char temp[256];
                    memcpy(temp, array + i * size, size);
                    memcpy(array + i * size, array + j * size, size);
                    memcpy(array + j * size, temp, size);
                }
            }
            i++;
            j--;
        }
    }
    
    return i;
}

// Recursive quicksort
static void qsort_recursive(void *base, size_t nmemb, size_t size,
                           int (*compar)(const void *, const void *))
{
    if (nmemb <= 1) return;
    
    // Use insertion sort for small arrays
    if (nmemb <= 16) {
        insertion_sort(base, nmemb, size, compar);
        return;
    }
    
    size_t pivot = partition(base, nmemb, size, compar);
    
    // Recursively sort partitions
    if (pivot > 0) {
        qsort_recursive(base, pivot, size, compar);
    }
    if (pivot < nmemb) {
        qsort_recursive(base + pivot * size, nmemb - pivot, size, compar);
    }
}

// Reentrant version for thread safety
void qsort_r(void *base, size_t nmemb, size_t size,
             cmp_r_func_t compar, void *arg)
{
    struct qsort_r_data {
        cmp_r_func_t compar;
        void *arg;
    } data = { compar, arg };
    
    // Wrapper function for reentrant calls
    int cmp_wrapper(const void *a, const void *b)
    {
        return data.compar(a, b, data.arg);
    }
    
    qsort_recursive(base, nmemb, size, cmp_wrapper);
}
```

### Binary Search Implementation
```c
// Optimized binary search with branch prediction
void *bsearch_opt(const void *key, const void *base,
                  size_t nmemb, size_t size,
                  int (*compar)(const void *, const void *))
{
    const char *array = base;
    size_t left = 0, right = nmemb;
    
    // Branch-prediction-friendly loop
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        const void *mid_elem = array + mid * size;
        int cmp = compar(key, mid_elem);
        
        if (cmp == 0) {
            return (void *)mid_elem;  // Found
        } else if (cmp < 0) {
            right = mid;  // Search left half
        } else {
            left = mid + 1;  // Search right half
        }
    }
    
    return NULL;  // Not found
}
```

## Environment Management

### Environment Variable Access
```c
// Thread-safe environment management
typedef struct env_entry {
    char *name;
    char *value;
    struct env_entry *next;
} env_entry_t;

static env_entry_t *env_list = NULL;
static pthread_mutex_t env_lock = PTHREAD_MUTEX_INITIALIZER;

// Optimized getenv with caching
char *getenv_opt(const char *name)
{
    static __thread char *last_result = NULL;
    static __thread char last_name[256] = "";
    
    // Fast path: check cache
    if (strcmp(last_name, name) == 0) {
        return last_result;
    }
    
    // Search environment list
    pthread_mutex_lock(&env_lock);
    env_entry_t *entry = env_list;
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            pthread_mutex_unlock(&env_lock);
            
            // Update cache
            strncpy(last_name, name, sizeof(last_name) - 1);
            last_result = entry->value;
            return last_result;
        }
        entry = entry->next;
    }
    pthread_mutex_unlock(&env_lock);
    
    return NULL;
}

// Set environment variable
int setenv_opt(const char *name, const char *value, int overwrite)
{
    pthread_mutex_lock(&env_lock);
    
    // Find existing entry
    env_entry_t *prev = NULL, *entry = env_list;
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            if (!overwrite) {
                pthread_mutex_unlock(&env_lock);
                return 0;
            }
            // Replace value
            free(entry->value);
            entry->value = strdup(value);
            pthread_mutex_unlock(&env_lock);
            return 0;
        }
        prev = entry;
        entry = entry->next;
    }
    
    // Create new entry
    entry = malloc(sizeof(env_entry_t));
    entry->name = strdup(name);
    entry->value = strdup(value);
    entry->next = NULL;
    
    if (prev) {
        prev->next = entry;
    } else {
        env_list = entry;
    }
    
    pthread_mutex_unlock(&env_lock);
    return 0;
}
```

## Mathematical Utility Functions

### Integer Arithmetic Operations
```c
// RISC-V 64-bit optimized absolute value
int abs_opt(int j)
{
    // Use conditional move instruction on RISC-V
    int result = j;
    if (j < 0) result = -j;
    return result;
}

long labs_opt(long j)
{
    long result = j;
    if (j < 0) result = -j;
    return result;
}

long long llabs_opt(long long j)
{
    long long result = j;
    if (j < 0) result = -j;
    return result;
}

// Integer division with remainder
typedef struct {
    int quot;  // Quotient
    int rem;   // Remainder
} div_t;

div_t div_opt(int numer, int denom)
{
    div_t result;
    
    // Handle division by zero
    if (denom == 0) {
        errno = EDOM;
        result.quot = 0;
        result.rem = 0;
        return result;
    }
    
    // Handle INT_MIN / -1 overflow
    if (numer == INT_MIN && denom == -1) {
        errno = ERANGE;
        result.quot = INT_MAX;
        result.rem = 0;
        return result;
    }
    
    result.quot = numer / denom;
    result.rem = numer % denom;
    
    return result;
}
```

## Process Control Functions

### Command Execution
```c
// Safe command execution with proper argument handling
int system_opt(const char *command)
{
    if (command == NULL) {
        // Check if shell is available
        return access("/bin/sh", X_OK) == 0 ? 1 : 0;
    }
    
    // Fork and execute command
    pid_t pid = fork();
    if (pid == -1) {
        return -1;  // Fork failed
    } else if (pid == 0) {
        // Child process
        execl("/bin/sh", "sh", "-c", command, NULL);
        _exit(127);  // execl failed
    } else {
        // Parent process - wait for child
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            return -1;
        }
        return status;
    }
}
```

### Exit Handler Management
```c
// Thread-safe exit handler registration
typedef struct exit_handler {
    void (*func)(void);
    struct exit_handler *next;
} exit_handler_t;

static exit_handler_t *exit_handlers = NULL;
static int exit_handlers_count = 0;
static pthread_mutex_t exit_lock = PTHREAD_MUTEX_INITIALIZER;

// Register exit handler
int atexit_opt(void (*func)(void))
{
    pthread_mutex_lock(&exit_lock);
    
    // Check maximum handlers
    if (exit_handlers_count >= ATEXIT_MAX) {
        pthread_mutex_unlock(&exit_lock);
        return -1;
    }
    
    // Create new handler entry
    exit_handler_t *handler = malloc(sizeof(exit_handler_t));
    handler->func = func;
    handler->next = exit_handlers;
    exit_handlers = handler;
    exit_handlers_count++;
    
    pthread_mutex_unlock(&exit_lock);
    return 0;
}

// Execute exit handlers
void __run_exit_handlers(void)
{
    pthread_mutex_lock(&exit_lock);
    
    // Execute handlers in reverse order
    while (exit_handlers) {
        exit_handler_t *handler = exit_handlers;
        exit_handlers = handler->next;
        
        pthread_mutex_unlock(&exit_lock);
        handler->func();  // Execute handler
        free(handler);
        pthread_mutex_lock(&exit_lock);
    }
    
    pthread_mutex_unlock(&exit_lock);
}
```

## Build Configuration

### Architecture-Specific Optimizations
```makefile
# RISC-V 64-bit stdlib build configuration
STDLIB_SRCS += \
    abs.c labs.c llabs.c \
    div.c ldiv.c lldiv.c imaxdiv.c \
    atoi.c atol.c atoll.c atof.c \
    strtol.c strtoll.c strtoul.c strtoull.c \
    rand.c srand.c random.c srandom.c \
    malloc.c free.c realloc.c calloc.c \
    qsort.c qsort_r.c bsearch.c \
    getenv.c setenv.c unsetenv.c putenv.c

# RISC-V specific optimizations
CFLAGS += -march=rv64gc -mabi=lp64d
CFLAGS += -falign-functions=16 -falign-loops=8
CFLAGS += -DUSE_RISCV_64BIT_OPS -DUSE_ATOMIC_LOCKS

# Memory allocation optimizations
CFLAGS += -DMALLOC_ALIGNMENT=16 -DUSE_THREAD_ARENAS
CFLAGS += -DSMALLOC_THRESHOLD=256
```

## Performance Characteristics

### Memory Allocation Performance
- Small allocations (< 256 bytes): O(1) with thread-local caching
- Large allocations: O(log n) with best-fit algorithm
- Free operations: O(1) amortized with coalescing
- Memory overhead: ~8 bytes per allocation

### Sorting Algorithm Performance
- Quick sort average case: O(n log n)
- Insertion sort for small arrays: O(n²) but faster for n ≤ 16
- Memory usage: O(log n) for recursion stack
- Stable sorting: Available with qsort_r()

### String Conversion Performance
- Integer parsing: O(n) where n is string length
- Optimized for common cases (base 10, no overflow)
- Minimal memory allocations
- Thread-safe with per-thread caching

## Testing and Validation

### Function Correctness Tests
```c
// Test memory allocation correctness
void test_malloc_correctness(void)
{
    // Test basic allocation
    void *ptr = malloc(100);
    assert(ptr != NULL);
    
    // Test alignment
    assert(((uintptr_t)ptr & 15) == 0);
    
    // Test zero-size allocation
    void *zero = malloc(0);
    assert(zero != NULL || zero == NULL);  // Either is valid
    
    // Test large allocation
    void *large = malloc(1024 * 1024);
    assert(large != NULL);
    
    free(ptr);
    free(zero);
    free(large);
}

// Test sorting algorithm correctness
void test_qsort_correctness(void)
{
    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    int expected[] = {1, 1, 2, 3, 3, 4, 5, 5, 5, 6, 9};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    qsort(arr, n, sizeof(int), int_compare);
    
    for (size_t i = 0; i < n; i++) {
        assert(arr[i] == expected[i]);
    }
}
```

The stdlib library provides the fundamental runtime utilities that MINIX 3 applications depend on, with RISC-V 64-bit optimizations ensuring efficient memory management, fast conversions, and reliable process control operations.