# MINIX Kernel Library Interface System

## Overview

The `sys/lib/` directory contains the kernel library interface system for MINIX 3, providing essential libraries and utilities that support kernel operations, standalone boot processes, exception handling, and compression. This directory serves as the foundational library layer that enables MINIX to operate efficiently across different hardware platforms while maintaining the modularity and reliability principles of the microkernel architecture.

## Purpose and Responsibilities

### Core Functions
- **Kernel Library Support**: Provides essential library functions for kernel operations
- **Standalone Boot Libraries**: Supports standalone boot and initialization processes
- **Exception Handling**: Offers stack unwinding and exception handling capabilities
- **Compression Support**: Provides zlib compression for kernel and system utilities
- **Cross-Platform Utilities**: Ensures consistent library interfaces across architectures

### Key Features
- **Minimal Kernel Libraries** - Essential functions without full libc overhead
- **Standalone Boot Support** - Libraries for pre-kernel environment
- **Exception Unwinding** - Stack trace and exception handling
- **Compression Integration** - zlib for kernel and boot compression
- **Architecture Independence** - Portable library implementations

## Directory Structure

```
sys/lib/
├── libkern/              # Kernel library functions
│   ├── Makefile          # Kernel library makefile
│   ├── _ashldi3.S        # Arithmetic shift left (64-bit)
│   ├── _ashrdi3.S        # Arithmetic shift right (64-bit)
│   ├── _cmpdi2.c         # Compare (64-bit)
│   ├── _divdi3.S         # Division (64-bit)
│   ├── _lshldi3.S        # Logical shift left (64-bit)
│   ├── _lshrdi3.S        # Logical shift right (64-bit)
│   ├── _moddi3.S         # Modulo (64-bit)
│   ├── _muldi3.S         # Multiplication (64-bit)
│   ├── _negdi2.S         # Negation (64-bit)
│   ├── _qdivrem.c        # Quotient and remainder
│   ├── _udivdi3.S        # Unsigned division (64-bit)
│   ├── _umoddi3.S        # Unsigned modulo (64-bit)
│   ├── bcmp.c            # Byte comparison
│   ├── bcopy.c           # Byte copy
│   ├── bzero.c           # Byte zero
│   ├── db_interface.c    # Debugger interface
│   ├── fabs.c            # Floating point absolute value
│   ├── floor.c           # Floor function
│   ├── index.c           # String index
│   ├── inet_ntop.c       # Internet address conversion
│   ├── inet_pton.c       # Internet address conversion
│   ├── libkern.h         # Kernel library header
│   ├── memchr.c          # Memory character search
│   ├── memcmp.c          # Memory comparison
│   ├── memcpy.c          # Memory copy
│   ├── memmove.c         # Memory move
│   ├── memset.c          # Memory set
│   ├── qsort.c           # Quick sort
│   ├── rindex.c          # Reverse string index
│   ├── snprintf.c        # Formatted output
│   ├── strchr.c          # String character search
│   ├── strcmp.c          # String comparison
│   ├── strcpy.c          # String copy
│   ├── strlen.c          # String length
│   ├── strncmp.c         # String comparison (limited)
│   ├── strncpy.c         # String copy (limited)
│   ├── strrchr.c         # Reverse string character search
│   ├── strstr.c          # String substring search
│   ├── subdi3.c          # Subtraction (64-bit)
│   ├── umul64.c          # Unsigned multiplication (64-bit)
│   └── udiv64.c          # Unsigned division (64-bit)
├── libsa/                # Standalone boot library
│   ├── Makefile          # Standalone library makefile
│   ├── Makefile.inc      # Standalone include makefile
│   ├── alloc.c           # Memory allocation
│   ├── arch/             # Architecture-specific code
│   │   ├── evbriscv64/   # RISC-V 64-bit support
│   │   ├── i386/         # x86 support
│   │   └── Makefile      # Architecture makefile
│   ├── assert.c          # Assertion handling
│   ├── boot.c            # Boot support
│   ├── close.c           # File close
│   ├── cread.c           # Compressed read
│   ├── ctype.c           # Character type
│   ├── dev.c             # Device interface
│   ├── disklabel.c       # Disk label support
│   ├── dvma.c            # DVMA support
│   ├── errno.c           # Error numbers
│   ├── exec.c            # Execution support
│   ├── gets.c            # String input
│   ├── getsn.c           # String input (limited)
│   ├── icmp.c            # ICMP support
│   ├── inet.c            # Internet support
│   ├── inet_ntoa.c       # Internet address conversion
│   ├── ip.c              # IP support
│   ├── lib.h             # Standalone library header
│   ├── llc.c             # LLC support
│   ├── loadfile.c        # File loading
│   ├── lseek.c           # File seeking
│   ├── memcpy.c          # Memory copy
│   ├── memset.c          # Memory set
│   ├── net.c             # Network support
│   ├── netdev.c          # Network device
│   ├── nullfs.c          # Null file system
│   ├── open.c            # File open
│   ├── printf.c          # Formatted output
│   ├── putchar.c         # Character output
│   ├── puts.c            # String output
│   ├── read.c            # File read
│   ├── readdir.c         # Directory reading
│   ├── reboot.c          # System reboot
│   ├── rev.c             # String reverse
│   ├── sash.c            # Standalone shell
│   ├── snprintf.c        # Formatted output (limited)
│   ├── sprintf.c         # Formatted output to string
│   ├── stand.h           # Standalone header
│   ├── stat.c            # File status
│   ├── stdio.c           # Standard I/O
│   ├── strcat.c          # String concatenation
│   ├── strchr.c          # String character search
│   ├── strcmp.c          # String comparison
│   ├── strcpy.c          # String copy
│   ├── strlen.c          # String length
│   ├── strncmp.c         # String comparison (limited)
│   ├── strncpy.c         # String copy (limited)
│   ├── strtol.c          # String to long
│   ├── strtoul.c         # String to unsigned long
│   ├── tcp.c             # TCP support
│   ├── tftp.c            # TFTP support
│   ├── time.c            # Time functions
│   ├── udp.c             # UDP support
│   ├── unlink.c          # File unlink
│   ├── util.c            # Utility functions
│   └── write.c           # File write
├── libunwind/            # Exception unwinding library
│   ├── Makefile          # Unwind library makefile
│   ├── dwarf_eh.h        # DWARF exception header
│   ├── dwarf.h           # DWARF definitions
│   ├── dwarf_read.c      # DWARF reading
│   ├── libunwind.c       # Unwind implementation
│   ├── libunwind.h       # Unwind header
│   ├── unwind.c          # Unwind interface
│   └── unwind.h          # Unwind header
└── libz/                 # Compression library
    ├── Makefile          # zlib makefile
    ├── adler32.c         # Adler-32 checksum
    ├── compress.c        # Compression
    ├── crc32.c           # CRC-32 checksum
    ├── crc32.h           # CRC-32 header
    ├── deflate.c         # Deflate compression
    ├── deflate.h         # Deflate header
    ├── infback.c         # Inflate back
    ├── inffast.c         # Fast inflate
    ├── inffast.h         # Fast inflate header
    ├── inffixed.h        # Fixed inflate tables
    ├── inflate.c         # Inflate decompression
    ├── inflate.h         # Inflate header
    ├── inftrees.c        # Inflate trees
    ├── inftrees.h        # Inflate trees header
    ├── trees.c           # Huffman trees
    ├── trees.h           # Trees header
    ├── uncompr.c         # Decompression
    ├── zconf.h           # zlib configuration
    ├── zlib.h            # zlib header
    └── zutil.c           # zlib utilities
```

## Kernel Library (libkern)

### Essential Kernel Functions
Minimal library functions for kernel operations without full libc overhead:

```c
/* Kernel library header (libkern.h) */
#ifndef _LIBKERN_H_
#define _LIBKERN_H_

/* Memory operations */
void *memcpy(void *dst, const void *src, size_t len);
void *memmove(void *dst, const void *src, size_t len);
void *memset(void *b, int c, size_t len);
int memcmp(const void *b1, const void *b2, size_t len);
void *memchr(const void *b, int c, size_t len);

/* String operations */
size_t strlen(const char *str);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t len);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t len);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *big, const char *little);
char *index(const char *s, int c);
char *rindex(const char *s, int c);

/* 64-bit arithmetic */
int64_t __ashldi3(int64_t a, int b);
int64_t __ashrdi3(int64_t a, int b);
int64_t __divdi3(int64_t a, int64_t b);
int64_t __lshldi3(int64_t a, int b);
int64_t __lshrdi3(int64_t a, int b);
int64_t __moddi3(int64_t a, int64_t b);
int64_t __muldi3(int64_t a, int64_t b);
int64_t __negdi2(int64_t a);
int64_t __subdi3(int64_t a, int64_t b);
uint64_t __udivdi3(uint64_t a, uint64_t b);
uint64_t __umoddi3(uint64_t a, uint64_t b);

/* Mathematical functions */
double fabs(double x);
double floor(double x);

/* Sorting */
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));

/* Network conversion */
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
int inet_pton(int af, const char *src, void *dst);

/* Debugging */
void db_enter(void);
void db_console(void);

#endif /* _LIBKERN_H_ */
```

### 64-bit Arithmetic Support
Essential 64-bit arithmetic operations for 32-bit architectures:

```c
/* 64-bit division and modulo (_qdivrem.c) */
union uu {
    struct {
        uint32_t lo;            /* Low 32 bits */
        uint32_t hi;            /* High 32 bits */
    } u_s;
    uint64_t u_q;               /* Full 64-bit value */
};

/* 64-bit division */
int64_t __divdi3(int64_t a, int64_t b) {
    union uu aa, bb, cc;
    int neg;
    
    aa.u_q = a;
    bb.u_q = b;
    neg = 0;
    
    if (aa.u_s.hi < 0) {
        neg = !neg;
        aa.u_q = -aa.u_q;
    }
    if (bb.u_s.hi < 0) {
        neg = !neg;
        bb.u_q = -bb.u_q;
    }
    
    cc = __qdivrem(aa, bb, NULL);
    
    if (neg)
        cc.u_q = -cc.u_q;
    
    return cc.u_q;
}

/* Unsigned 64-bit division */
uint64_t __udivdi3(uint64_t a, uint64_t b) {
    union uu aa, cc;
    
    aa.u_q = a;
    cc = __qdivrem(aa, (union uu)b, NULL);
    
    return cc.u_q;
}
```

### Memory and String Operations
Optimized memory and string operations for kernel use:

```c
/* Optimized memory operations */
void *memcpy(void *dst, const void *src, size_t len) {
    char *d = dst;
    const char *s = src;
    
    while (len-- > 0)
        *d++ = *s++;
    
    return dst;
}

void *memset(void *b, int c, size_t len) {
    char *p = b;
    
    while (len-- > 0)
        *p++ = c;
    
    return b;
}

int memcmp(const void *b1, const void *b2, size_t len) {
    const char *p1 = b1, *p2 = b2;
    
    while (len-- > 0) {
        if (*p1 != *p2)
            return *p1 - *p2;
        p1++, p2++;
    }
    
    return 0;
}

/* String operations */
size_t strlen(const char *str) {
    const char *s;
    
    for (s = str; *s; ++s)
        continue;
    
    return s - str;
}

char *strcpy(char *dst, const char *src) {
    char *save = dst;
    
    while ((*dst++ = *src++) != 0)
        continue;
    
    return save;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 == *s2++)
        if (*s1++ == 0)
            return 0;
    
    return *(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1);
}
```

## Standalone Boot Library (libsa)

### Standalone Environment Support
Library functions for pre-kernel boot environment:

```c
/* Standalone library header (lib.h) */
#ifndef _LIB_H_
#define _LIB_H_

/* Standalone environment definitions */
#define RB_AUTOBOOT     0       /* Use default boot */
#define RB_ASKNAME      0x01    /* Ask for file name */
#define RB_SINGLE       0x02    /* Boot to single user */
#define RB_NOSYNC       0x04    /* Don't sync before reboot */
#define RB_HALT         0x08    /* Don't reboot, just halt */
#define RB_INITNAME     0x10    /* Use given init name */
#define RB_DFLTROOT     0x20    /* Use default root */
#define RB_KDB          0x40    /* Give control to kernel debugger */
#define RB_RDONLY       0x80    /* Mount root read-only */
#define RB_DUMP         0x100   /* Dump kernel memory */
#define RB_MINIROOT     0x200   /* Use miniroot */
#define RB_STRING       0x400   /* Use boot string */

/* File operations */
int open(const char *path, int mode);
int close(int fd);
ssize_t read(int fd, void *buf, size_t nbytes);
ssize_t write(int fd, const void *buf, size_t nbytes);
off_t lseek(int fd, off_t offset, int whence);
int stat(const char *path, struct stat *sb);

/* Memory allocation */
void *alloc(size_t size);
void free(void *ptr);

/* String formatting */
int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
int snprintf(char *buf, size_t size, const char *fmt, ...);

/* String operations */
char *gets(char *buf);
char *getsn(char *buf, size_t n);

/* Utility functions */
void panic(const char *fmt, ...);
int getchar(void);
int putchar(int c);
int puts(const char *s);
uint32_t crc32(const void *buf, size_t len);

/* Time functions */
time_t time(time_t *tloc);

/* Network support */
int inet_aton(const char *cp, struct in_addr *addr);
char *inet_ntoa(struct in_addr in);

#endif /* _LIB_H_ */
*/
```

### Network Boot Support
Network protocols for network booting:

```c
/* TFTP support (tftp.c) */
#define TFTP_PORT       69      /* TFTP port */
#define TFTP_REXMT      5       /* Retransmission timeout */
#define TFTP_MAXTIMEOUT 255     /* Maximum timeout */

/* TFTP operations */
#define TFTP_RRQ        1       /* Read request */
#define TFTP_WRQ        2       /* Write request */
#define TFTP_DATA       3       /* Data */
#define TFTP_ACK        4       /* Acknowledgment */
#define TFTP_ERROR      5       /* Error */

/* TFTP error codes */
#define TFTP_ENOTDEFINED 0      /* Not defined */
#define TFTP_EFILENOTFOUND 1    /* File not found */
#define TFTP_EACCESS    2       /* Access violation */
#define TFTP_ENOSPACE   3       /* Disk full */
#define TFTP_EILLEGAL   4       /* Illegal operation */
#define TFTP_EUNKNOWNID 5       /* Unknown transfer ID */
#define TFTP_EEXISTS    6       /* File already exists */
#define TFTP_ENOUSER    7       /* No such user */

/* TFTP file operations */
int tftp_open(const char *name, const char *mode);
int tftp_close(int fd);
ssize_t tftp_read(int fd, void *buf, size_t nbytes);
ssize_t tftp_write(int fd, const void *buf, size_t nbytes);
```

### Boot File Loading
File loading support for boot process:

```c
/* Load file support (loadfile.c) */
#define LOAD_ALL        0x01    /* Load all sections */
#define LOAD_KERNEL     0x02    /* Load kernel only */
#define LOAD_SYMBOLS    0x04    /* Load symbols */
#define LOAD_STRINGS    0x08    /* Load string table */
#define LOAD_HDR        0x10    /* Load header only */

/* File loading */
int loadfile(const char *filename, uint64_t *marks, int flags);
int loadfile_raw(const char *filename, void *buf, size_t bufsize, int flags);

/* File format support */
#define LOAD_ELF        0x01    /* ELF format */
#define LOAD_AOUT       0x02    /* a.out format */
#define LOAD_RAW        0x04    /* Raw format */
```

## Exception Unwinding (libunwind)

### Stack Unwinding Support
Exception handling and stack trace support:

```c
/* Unwind interface (unwind.h) */
#ifndef _UNWIND_H_
#define _UNWIND_H_

/* Unwind reason codes */
typedef enum {
    _URC_NO_REASON = 0,
    _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
    _URC_FATAL_PHASE2_ERROR = 2,
    _URC_FATAL_PHASE1_ERROR = 3,
    _URC_NORMAL_STOP = 4,
    _URC_END_OF_STACK = 5,
    _URC_HANDLER_FOUND = 6,
    _URC_INSTALL_CONTEXT = 7,
    _URC_CONTINUE_UNWIND = 8
} _Unwind_Reason_Code;

/* Unwind context */
struct _Unwind_Context;

/* Unwind actions */
typedef int _Unwind_Action;
#define _UA_SEARCH_PHASE    1
#define _UA_CLEANUP_PHASE   2
#define _UA_HANDLER_FRAME   4
#define _UA_FORCE_UNWIND    8
#define _UA_END_OF_STACK    16

/* Exception cleanup */
typedef void (*_Unwind_Exception_Cleanup_Fn)(_Unwind_Reason_Code reason,
                                             struct _Unwind_Exception *exc);

/* Unwind functions */
_Unwind_Reason_Code _Unwind_RaiseException(struct _Unwind_Exception *exception_object);
_Unwind_Reason_Code _Unwind_ForcedUnwind(struct _Unwind_Exception *exception_object,
                                         _Unwind_Stop_Fn stop,
                                         void *stop_parameter);
void _Unwind_Resume(struct _Unwind_Exception *exception_object);
void _Unwind_DeleteException(struct _Unwind_Exception *exception_object);

#endif /* _UNWIND_H_ */
```

### DWARF Exception Handling
DWARF-based exception handling support:

```c
/* DWARF definitions (dwarf.h) */
#define DW_EH_PE_absptr     0x00    /* Absolute pointer */
#define DW_EH_PE_uleb128    0x01    /* Unsigned LEB128 */
#define DW_EH_PE_udata2     0x02    /* Unsigned 2 bytes */
#define DW_EH_PE_udata4     0x03    /* Unsigned 4 bytes */
#define DW_EH_PE_udata8     0x04    /* Unsigned 8 bytes */
#define DW_EH_PE_sleb128    0x09    /* Signed LEB128 */
#define DW_EH_PE_sdata2     0x0A    /* Signed 2 bytes */
#define DW_EH_PE_sdata4     0x0B    /* Signed 4 bytes */
#define DW_EH_PE_sdata8     0x0C    /* Signed 8 bytes */
#define DW_EH_PE_pcrel      0x10    /* PC relative */
#define DW_EH_PE_textrel    0x20    /* Text relative */
#define DW_EH_PE_datarel    0x30    /* Data relative */
#define DW_EH_PE_funcrel    0x40    /* Function relative */
#define DW_EH_PE_aligned    0x50    /* Aligned */
#define DW_EH_PE_indirect   0x80    /* Indirect */

/* DWARF call frame information */
struct dwarf_cie {
    uint32_t    length;         /* CIE length */
    uint32_t    cie_id;         /* CIE ID */
    uint8_t     version;        /* Version */
    uint8_t     augmentation[1]; /* Augmentation string */
    /* Variable length fields follow */
};

struct dwarf_fde {
    uint32_t    length;         /* FDE length */
    uint32_t    cie_offset;     /* CIE offset */
    uint32_t    pc_begin;       /* PC begin */
    uint32_t    pc_range;       /* PC range */
    /* Variable length fields follow */
};
```

## Compression Library (libz)

### zlib Interface
Standard zlib compression library:

```c
/* zlib header (zlib.h) */
#ifndef _ZLIB_H_
#define _ZLIB_H_

/* zlib version */
#define ZLIB_VERSION "1.2.11"
#define ZLIB_VERNUM 0x1280

/* Compression levels */
#define Z_NO_COMPRESSION      0
#define Z_BEST_SPEED          1
#define Z_BEST_COMPRESSION    9
#define Z_DEFAULT_COMPRESSION (-1)

/* Compression strategy */
#define Z_FILTERED        1
#define Z_HUFFMAN_ONLY    2
#define Z_RLE             3
#define Z_FIXED           4
#define Z_DEFAULT_STRATEGY 0

/* Return codes */
#define Z_OK              0
#define Z_STREAM_END      1
#define Z_NEED_DICT       2
#define Z_ERRNO          (-1)
#define Z_STREAM_ERROR   (-2)
#define Z_DATA_ERROR     (-3)
#define Z_MEM_ERROR      (-4)
#define Z_BUF_ERROR      (-5)
#define Z_VERSION_ERROR  (-6)

/* Compression methods */
#define Z_DEFLATED   8

/* Data types */
typedef void *z_streamp;
typedef unsigned char Byte;
typedef unsigned int uInt;
typedef unsigned long uLong;

/* z_stream structure */
typedef struct z_stream_s {
    Byte     *next_in;      /* Next input byte */
    uInt     avail_in;      /* Number of bytes available at next_in */
    uLong    total_in;      /* Total number of input bytes read */
    
    Byte     *next_out;     /* Next output byte */
    uInt     avail_out;     /* Remaining free space at next_out */
    uLong    total_out;     /* Total number of bytes output */
    
    char     *msg;          /* Last error message */
    struct internal_state *state; /* Not visible by applications */
    
    alloc_func zalloc;      /* Used to allocate internal memory */
    free_func  zfree;       /* Used to free internal memory */
    voidpf     opaque;      /* Private data object */
    
    int     data_type;      /* Best guess about data type */
    uLong   adler;          /* Adler-32 or CRC-32 value */
    uLong   reserved;       /* Reserved for future use */
} z_stream;

/* Basic functions */
const char *zlibVersion(void);
int deflate(z_streamp strm, int flush);
int deflateEnd(z_streamp strm);
int inflate(z_streamp strm, int flush);
int inflateEnd(z_streamp strm);

#endif /* _ZLIB_H_ */
*/
```

### Compression Implementation
Core compression algorithms:

```c
/* Deflate compression (deflate.c) */
struct deflate_state {
    z_stream *strm;         /* Pointer back to this stream */
    int status;             /* Current status */
    Byte *pending_buf;      /* Output still pending */
    ulg pending_buf_size;   /* Size of pending buffer */
    Byte *pending_out;      /* Next pending byte */
    int pending;            /* Number of pending bytes */
    
    /* Sliding window */
    Byte *window;           /* Sliding window */
    uInt window_size;       /* Size of sliding window */
    
    /* Hash table */
    Pos *head;              /* Hash table */
    uInt hash_size;         /* Hash table size */
    uInt hash_bits;         /* Hash table bits */
    uInt hash_mask;         /* Hash table mask */
    
    /* Match finding */
    uInt hash_shift;        /* Hash shift */
    long block_start;       /* Window position */
    uInt match_length;      /* Length of best match */
    IPos prev_match;        /* Previous match */
    int match_available;    /* Match available */
    uInt strstart;          /* Start of string */
    uInt match_start;       /* Start of match */
    uInt lookahead;         /* Lookahead */
    uInt prev_length;       /* Previous match length */
    
    /* Compression level */
    int level;              /* Compression level */
    int strategy;           /* Compression strategy */
    uInt good_match;        /* Good match */
    int nice_match;         /* Nice match */
    
    /* Huffman coding */
    struct huft *l_tree;    /* Literal tree */
    struct huft *d_tree;    /* Distance tree */
    struct huft *bl_tree;   /* Bit length tree */
    
    /* Dynamic tables */
    ush bl_count[MAX_BITS+1]; /* Bit length count */
    int heap[2*L_CODES+1];  /* Heap */
    int heap_len;           /* Heap length */
    int heap_max;           /* Heap maximum */
    uch depth[2*L_CODES+1]; /* Depth */
    
    /* Encoding */
    uch *l_buf;             /* Literal buffer */
    u32 lit_bufsize;        /* Literal buffer size */
    uInt last_lit;          /* Last literal */
    ush *d_buf;             /* Distance buffer */
    ulg opt_len;            /* Optimal length */
    ulg static_len;         /* Static length */
    uInt matches;           /* Number of matches */
    int last_eob_len;       /* Last EOB length */
    uch bi_buf;             /* Bit buffer */
    int bi_valid;           /* Valid bits in bi_buf */
    
    /* High water mark */
    ulg high_water;         /* High water mark */
};
```

## Build System Integration

### Library Build Configuration
Integration with MINIX build system:

```makefile
# sys/lib/Makefile
# Kernel library build configuration

# Kernel library
SUBDIR += libkern

# Standalone library
SUBDIR += libsa

# Unwind library
SUBDIR += libunwind

# zlib
SUBDIR += libz

# Cross-compilation support
.if defined(MACHINE_ARCH)
CFLAGS += -I${.CURDIR}/libkern
CFLAGS += -I${.CURDIR}/libsa
CFLAGS += -I${.CURDIR}/libunwind
CFLAGS += -I${.CURDIR}/libz
.endif
```

### Architecture-Specific Optimizations
Architecture-specific library optimizations:

```makefile
# Architecture-specific optimizations
.if ${MACHINE_ARCH} == "evbriscv64"
# RISC-V specific optimizations
libkern/_ashldi3.S: libkern/arch/riscv64/_ashldi3.S
libkern/_ashrdi3.S: libkern/arch/riscv64/_ashrdi3.S
.elif ${MACHINE_ARCH} == "i386"
# x86 specific optimizations
libkern/_ashldi3.S: libkern/arch/i386/_ashldi3.S
libkern/_ashrdi3.S: libkern/arch/i386/_ashrdi3.S
.elif ${MACHINE_ARCH} == "arm"
# ARM specific optimizations
libkern/_ashldi3.S: libkern/arch/arm/_ashldi3.S
libkern/_ashrdi3.S: libkern/arch/arm/_ashrdi3.S
.endif
```

## Testing and Validation

### Library Testing Framework
Comprehensive testing for library functions:

```c
/* Library test framework */
struct lib_test_case {
    const char *name;               /* Test name */
    int (*test_func)(void);         /* Test function */
    int expected_result;            /* Expected result */
};

/* Kernel library tests */
static struct lib_test_case kern_tests[] = {
    { "memcpy", test_memcpy, 0 },
    { "memcmp", test_memcmp, 0 },
    { "strlen", test_strlen, 0 },
    { "strcmp", test_strcmp, 0 },
    { "64bit_div", test_64bit_div, 0 },
    { "64bit_mul", test_64bit_mul, 0 },
    /* ... additional tests ... */
};

/* Standalone library tests */
static struct lib_test_case sa_tests[] = {
    { "file_open", test_file_open, 0 },
    { "file_read", test_file_read, 0 },
    { "memory_alloc", test_memory_alloc, 0 },
    { "network_boot", test_network_boot, 0 },
    /* ... additional tests ... */
};
```

### Cross-Architecture Validation
Ensures library functions work across architectures:

```c
/* Cross-architecture library validation */
int test_cross_arch_libraries(void) {
    int result = 0;
    
    /* Test 64-bit arithmetic across architectures */
    result |= test_64bit_arithmetic();
    
    /* Test memory operations */
    result |= test_memory_operations();
    
    /* Test string operations */
    result |= test_string_operations();
    
    /* Test compression */
    result |= test_compression();
    
    return result;
}
```

The kernel library interface system provides the essential foundation that enables MINIX 3 to operate efficiently across different hardware platforms while maintaining the modularity and reliability principles of the microkernel architecture. This comprehensive library framework ensures consistent and reliable system operations from boot through runtime execution.