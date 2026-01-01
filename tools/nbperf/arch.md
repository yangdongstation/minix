# MINIX 3 nbperf Tool - Architecture Documentation

## Overview

The `tools/nbperf/` directory provides the `nbperf` utility, a sophisticated perfect hash function generator that creates optimal hash functions for static sets of keys. This tool is essential for MINIX 3's system libraries and utilities that require fast, collision-free string lookups, such as keyword recognition, command parsing, and symbol table implementations. For cross-compilation scenarios, particularly RISC-V development, it ensures that perfect hash functions are generated efficiently on the host system for use on the target architecture.

## Purpose and Role in Build System

`nbperf` generates perfect hash functions that map a static set of keys to consecutive integers without collisions, providing O(1) lookup performance with minimal memory overhead. This tool is crucial for building efficient system utilities, library functions, and kernel components that need fast string-to-integer mappings, such as system call dispatch tables, command parsers, and configuration keyword recognizers.

## Directory Structure

```
tools/nbperf/
├── Makefile                     # Build configuration for host tool
└── obj/                        # Build artifacts directory
```

## Source Architecture

### Primary Source Location
The actual `nbperf` source code resides in:
```
usr.bin/nbperf/
├── nbperf.c                    # Main implementation and control
├── nbperf.h                    # Main header with data structures
├── nbperf.1                    # Manual page (4KB)
├── nbperf-bdz.c                # BDZ (Bit, Digit, XOR) algorithm
├── nbperf-chm.c                # CHM (Compress, Hash, and Displace)
├── nbperf-chm3.c               # CHM variant for 3-universal hashing
├── graph2.c                    # 2-regular graph construction
├── graph2.h                    # 2-regular graph interface
├── graph3.c                    # 3-regular graph construction
├── graph3.h                    # 3-regular graph interface
├── Makefile                    # Standard NetBSD makefile
└── obj/                        # Object files
```

### Build Tool Integration
The `tools/nbperf/Makefile` serves as a wrapper that:
- Defines `HOSTPROGNAME=${_TOOL_PREFIX}perf` (note: "nbperf" becomes "perf" with prefix)
- References `HOST_SRCDIR=usr.bin/nbperf`
- Includes `Makefile.host` for host tool building

## Core Functionality

### Perfect Hash Function Generation Process

1. **Key Set Analysis** - Analyze input keys for characteristics
2. **Algorithm Selection** - Choose appropriate perfect hashing algorithm
3. **Hash Function Construction** - Build primary and secondary hash functions
4. **Table Generation** - Create lookup tables and displacement vectors
5. **Code Generation** - Output C code for the perfect hash function
6. **Validation** - Verify perfect hashing properties

### Supported Algorithms

**BDZ Algorithm (Bit, Digit, XOR):**
- Based on random 2-regular graphs
- Simple and fast construction
- Good for small to medium key sets
- Memory efficient implementation

**CHM Algorithm (Compress, Hash, and Displace):**
- Uses 2-universal hashing
- Handles larger key sets efficiently
- Provides theoretical guarantees
- More complex but powerful

**CHM3 Algorithm (3-universal variant):**
- Uses 3-universal hashing
- Better compression ratios
- Handles very large key sets
- Advanced displacement techniques

### Input Format

**Key File Format:**
```
keyword1
keyword2
keyword3
# Comments supported
longer_keyword
mixed_Case
underscore_word
```

**Command-Line Options:**
```bash
nbperf [-bcdDghilnoprs] [-a algorithm] [-f keyfile] [-F functions] [-k keys] [-m minimum]
```

## Implementation Details

### Main Components (`nbperf.c`)

**Core Functions:**
- `main()` - Command-line processing and algorithm selection
- `read_keys()` - Parse input key file
- `generate_bdz()` - BDZ algorithm implementation
- `generate_chm()` - CHM algorithm implementation
- `generate_chm3()` - CHM3 algorithm implementation
- `output_code()` - Generate C code output

**Data Structures:**
```c
struct key {
    char *name;                 /* Key string */
    int len;                    /* Key length */
    int hash;                   /* Computed hash value */
    struct key *next;           /* Hash chain */
};

struct nbperf {
    struct key **keys;          /* Key array */
    int nkeys;                  /* Number of keys */
    int maxkeys;                /* Maximum keys allocated */
    int algorithm;              /* Selected algorithm */
    /* Algorithm-specific data */
};
```

### BDZ Algorithm (`nbperf-bdz.c`)

**Algorithm Overview:**
BDZ uses a random 2-regular graph structure where:
1. Keys are mapped to edges in a 2-regular graph
2. Each edge connects two vertices
3. A XOR operation on vertex values gives perfect hashing
4. Graph construction ensures no cycles of length 2

**Implementation Details:**
```c
static void
generate_bdz(struct nbperf *nbperf)
{
    struct bdz_graph *graph;
    int *vertices;
    int nvertices;
    
    /* Build 2-regular graph */
    graph = bdz_build_graph(nbperf);
    
    /* Find vertex values that satisfy XOR constraints */
    vertices = bdz_find_values(graph, nbperf);
    
    /* Generate C code for hash function */
    bdz_output_code(nbperf, graph, vertices);
}
```

### CHM Algorithm (`nbperf-chm.c`)

**Algorithm Overview:**
CHM uses 2-universal hashing with displacement tables:
1. Primary hash function maps keys to buckets
2. Secondary hash functions handle collisions
3. Displacement table resolves remaining conflicts
4. Guarantees O(1) lookup with minimal space

**Key Components:**
```c
struct chm_data {
    int *displ;                 /* Displacement table */
    int *bucket;                /* Bucket assignments */
    int nbuckets;               /* Number of buckets */
    int *hash_a, *hash_b;       /* Hash function parameters */
};
```

### Graph Construction (`graph2.c`, `graph3.c`)

**2-Regular Graph Construction:**
```c
struct graph2 {
    int *edge_src;              /* Source vertices */
    int *edge_dst;              /* Destination vertices */
    int *vertex_degree;         /* Vertex degrees */
    int nedges;                 /* Number of edges */
    int nvertices;              /* Number of vertices */
};
```

**Graph Properties:**
- Random permutation generation
- Degree constraint satisfaction
- Cycle detection and elimination
- Connected component analysis

## Cross-Compilation Architecture

### Host Tool Design
`nbperf` is built as a **host tool**:
- Compiled for the development host architecture
- Executes during library and utility construction
- Not installed on the target MINIX system

### Build System Integration

**Makefile Configuration:**
```makefile
HOSTPROGNAME= ${_TOOL_PREFIX}perf  # Results in "nbperf"
HOST_SRCDIR= usr.bin/nbperf
.include "${.CURDIR}/../Makefile.host"
```

**Algorithm Selection:**
- Automatic algorithm choice based on key set size
- Manual override via command-line options
- Cross-platform compatibility
- Host-independent code generation

### RISC-V Cross-Compilation Support

**Generated Code Portability:**
- Output C code is architecture-independent
- Works on RISC-V 32-bit and 64-bit systems
- No assembly language dependencies
- Standard C library usage only

**Build Workflow for RISC-V:**
```bash
# Build nbperf for host
./build.sh -m evbriscv64 tools

# Generate perfect hash function for RISC-V target
nbperf -f keywords.txt -o hashfunc.c

# Compile generated code for RISC-V
gcc -march=rv64gc -mabi=lp64d -c hashfunc.c
```

## Algorithm Details

### BDZ (Bit, Digit, XOR) Algorithm

**Theoretical Foundation:**
- Based on random 2-regular graphs
- Each key corresponds to a graph edge
- Vertex values found by solving XOR equations
- Perfect hashing guaranteed with high probability

**Construction Process:**
```
1. Generate random 2-regular graph with n vertices
2. Map each key to an edge in the graph
3. Assign random values to vertices
4. Solve XOR equations for consistent vertex values
5. Output hash function using vertex values
```

**Hash Function Form:**
```c
unsigned int
perfect_hash(const char *key, int len)
{
    unsigned int h1, h2;
    
    h1 = hash_function1(key, len) % nvertices;
    h2 = hash_function2(key, len) % nvertices;
    
    return vertex_values[h1] ^ vertex_values[h2];
}
```

### CHM (Compress, Hash, and Displace) Algorithm

**Two-Level Hashing:**
```
Level 1: Primary hash to buckets
Level 2: Secondary hash within buckets
Level 3: Displacement table for remaining conflicts
```

**Construction Algorithm:**
```
1. Choose random hash functions h1, h2
2. Map keys to primary buckets using h1
3. For each bucket, try secondary hash functions
4. Build displacement table for remaining items
5. Output multi-level hash function
```

**Hash Function Form:**
```c
unsigned int
perfect_hash(const char *key, int len)
{
    unsigned int primary, secondary;
    int bucket, displacement;
    
    primary = primary_hash(key, len);
    bucket = primary % nbuckets;
    
    if (bucket_size[bucket] == 1)
        return bucket;
    
    secondary = secondary_hash(key, len);
    displacement = displ[bucket][secondary % bucket_size[bucket]];
    
    return bucket + displacement;
}
```

### CHM3 (3-universal) Algorithm

**Enhanced Compression:**
- Uses 3-universal hash functions
- Better space utilization
- Handles larger key sets
- More complex construction

**Advantages:**
- Reduced memory requirements
- Faster construction for large sets
- Better theoretical bounds
- Improved practical performance

## Command-Line Interface

### Basic Usage
```bash
nbperf [-a algorithm] [-f keyfile] [-o output] [-k keys]
```

**Options:**
- `-a algorithm` - Select algorithm (bdz, chm, chm3)
- `-f keyfile` - Read keys from file (default: stdin)
- `-o output` - Write output to file (default: stdout)
- `-k keys` - Process only first N keys
- `-m minimum` - Minimum table size
- `-M maximum` - Maximum table size
- `-s seed` - Random seed for reproducibility

### Algorithm Selection

**Automatic Selection:**
```bash
nbperf -f keywords.txt           # Algorithm chosen automatically
```

**Manual Selection:**
```bash
nbperf -a bdz -f keywords.txt    # Force BDZ algorithm
nbperf -a chm -f keywords.txt    # Force CHM algorithm
nbperf -a chm3 -f keywords.txt   # Force CHM3 algorithm
```

### Advanced Options

**Performance Tuning:**
```bash
nbperf -f keywords.txt -m 1024   # Minimum table size 1024
nbperf -f keywords.txt -M 8192   # Maximum table size 8192
nbperf -f keywords.txt -s 12345  # Fixed random seed
```

**Debugging and Verification:**
```bash
nbperf -v -f keywords.txt        # Verbose output
nbperf -d -f keywords.txt        # Debug mode
nbperf -t -f keywords.txt        # Test generated function
```

## Generated Code Structure

### Output Format

**Header and Declarations:**
```c
/* Perfect hash function generated by nbperf */
#include <string.h>

static const unsigned int table[] = {
    0x12345678, 0x9abcdef0, /* ... table data ... */
};
```

**Hash Function Implementation:**
```c
unsigned int
perfect_hash(const char *key, int len)
{
    unsigned int h1, h2, h3;
    
    /* Primary hash computation */
    h1 = len;
    for (int i = 0; i < len; i++) {
        h1 = (h1 << 5) + h1 + key[i];
    }
    
    /* Algorithm-specific processing */
    h2 = table[h1 % TABLE_SIZE];
    h3 = table[h2 ^ (h1 >> 16)];
    
    return h3 % NUM_KEYS;
}
```

**Key Recognition Function:**
```c
const char *
perfect_key(unsigned int n)
{
    if (n >= NUM_KEYS) return NULL;
    return key_table[n];
}
```

### Integration Example

**System Call Dispatch:**
```c
/* Generated perfect hash function */
#include "syscall_hash.h"

static int (*syscall_table[])(void) = {
    sys_read, sys_write, sys_open, sys_close, /* ... */
};

int
syscall_dispatch(const char *name)
{
    unsigned int index;
    
    index = perfect_hash(name, strlen(name));
    if (index >= NUM_SYSCALLS) return -1;
    
    if (strcmp(name, perfect_key(index)) != 0) return -1;
    
    return syscall_table[index]();
}
```

## Performance Characteristics

### Construction Performance

**Time Complexity:**
- BDZ: O(n) expected time
- CHM: O(n log n) expected time
- CHM3: O(n log n) expected time
- n = number of keys

**Space Complexity:**
- BDZ: O(n) space for tables
- CHM: O(n) space for displacement tables
- CHM3: O(n) space with better constants

### Lookup Performance

**Hash Function Evaluation:**
- BDZ: 2-3 hash operations, 1 XOR
- CHM: 2-3 hash operations, 1 table lookup
- CHM3: 3-4 hash operations, 2 table lookups
- All provide O(1) lookup time

**Memory Access:**
- BDZ: 2 table accesses
- CHM: 2-3 table accesses
- CHM3: 3-4 table accesses
- All accesses are sequential or localized

## Build Dependencies

### Required Components
- Host C compiler
- `libnbcompat` for platform compatibility
- Standard C library functions
- Mathematical functions (random number generation)

### Optional Features
- Cryptographic hash functions (MD5, SHA-1, SHA-256)
- Large file support for big key sets
- Advanced random number generators
- Statistical analysis tools

## Testing and Validation

### Correctness Verification

**Perfect Property Check:**
```c
static int
verify_perfect_hash(struct nbperf *nbperf)
{
    int *used = calloc(nbperf->nkeys, sizeof(int));
    int collisions = 0;
    
    for (int i = 0; i < nbperf->nkeys; i++) {
        unsigned int h = perfect_hash(nbperf->keys[i]->name,
                                     nbperf->keys[i]->len);
        if (h >= nbperf->nkeys || used[h]) {
            collisions++;
        }
        used[h] = 1;
    }
    
    free(used);
    return collisions == 0;
}
```

**Key Recognition Test:**
- Verify reverse mapping correctness
- Check boundary conditions
- Validate string comparisons
- Test hash distribution

### Cross-Platform Testing

**Generated Code Validation:**
- Compile on different architectures
- Test endianness independence
- Verify word size compatibility
- RISC-V specific validation

**Algorithm Consistency:**
- Same input produces same output
- Cross-platform reproducibility
- Random seed effectiveness
- Statistical property verification

## Development Guidelines

### Key Set Preparation

**Key Selection:**
1. Choose stable, unchanging key sets
2. Avoid keys with common prefixes
3. Consider case sensitivity requirements
4. Validate key uniqueness
5. Test with realistic data sizes

**Key Format:**
```c
/* Example: System call names */
static const char *syscall_names[] = {
    "read", "write", "open", "close", "stat", "fstat", "lstat",
    "poll", "lseek", "mmap", "mprotect", "munmap", "brk", "rt_sigaction",
    /* ... complete list ... */
};
```

### Integration Best Practices

**Static Initialization:**
```c
/* Include generated code */
#include "generated_hash.h"

/* Static lookup table */
static const struct {
    const char *name;
    int value;
} lookup_table[] = {
    #include "key_values.h"
};

int
lookup_function(const char *key)
{
    unsigned int index;
    
    index = perfect_hash(key, strlen(key));
    if (index >= NUM_KEYS) return -1;
    
    if (strcmp(key, lookup_table[index].name) != 0) return -1;
    
    return lookup_table[index].value;
}
```

**Dynamic Loading:**
```c
/* Runtime hash function */
typedef struct {
    const char *key;
    int value;
} entry_t;

static entry_t *hash_table;
static int (*hash_func)(const char *, int);

void
initialize_hash(const char *keys[], int nkeys)
{
    /* Generate perfect hash at runtime */
    nbperf_generate(keys, nkeys, &hash_func, &hash_table);
}
```

## RISC-V Specific Considerations

### Target Architecture Optimization

**Code Generation for RISC-V:**
```c
/* RISC-V friendly hash function */
unsigned int
riscv_hash(const char *key, int len)
{
    register unsigned int h1 asm("a0") = 0;
    register unsigned int h2 asm("a1") = len;
    register const char *p asm("a2") = key;
    
    /* Use RISC-V specific instructions */
    asm volatile(
        "1: lb a3, 0(a2)\n"
        "   beqz a3, 2f\n"
        "   add a2, a2, 1\n"
        "   slli a4, a1, 5\n"
        "   add a1, a1, a4\n"
        "   add a1, a1, a3\n"
        "   j 1b\n"
        "2:\n"
        : "+r"(h1), "+r"(h2), "+r"(p)
        :
        : "a3", "a4", "memory"
    );
    
    return table[h1 & TABLE_MASK];
}
```

**Cross-Compilation Workflow:**
```bash
# Generate perfect hash on host
nbperf -f riscv_keywords.txt -o riscv_hash.c

# Compile for RISC-V target
riscv64-unknown-elf-gcc -march=rv64gc -mabi=lp64d \
    -O2 -c riscv_hash.c -o riscv_hash.o

# Link with MINIX kernel
ld riscv_hash.o kernel.o -o minix-kernel
```

### Performance on RISC-V

**Instruction Count Optimization:**
- Minimize hash computation instructions
- Use RISC-V compressed instructions (RVC)
- Optimize for RISC-V pipeline characteristics
- Leverage RISC-V multiply/divide unit

**Memory Access Patterns:**
- Sequential table access for cache efficiency
- Align tables for RISC-V memory subsystem
- Minimize TLB pressure
- Optimize for RISC-V cache sizes

This tool is fundamental to MINIX's performance-critical components, enabling efficient string lookups that are essential for system calls, library functions, and utility programs while maintaining full compatibility with cross-compilation environments and RISC-V target architectures.