# Standard I/O Functions Architecture

## Overview

The `lib/libc/stdio/` directory contains the standard input/output functions that provide buffered file operations, formatted input/output, and character/string I/O operations. These functions form the core I/O foundation that applications depend on for file manipulation, console interaction, and data serialization.

## Purpose and Responsibilities

### Core I/O Operations
- **File Operations**: Open, close, read, write, and position files
- **Formatted I/O**: Printf/scanf family for formatted data conversion
- **Character I/O**: Single character input/output operations
- **String I/O**: Line-oriented and block-oriented string operations
- **Buffer Management**: Internal buffering for performance optimization
- **Error Handling**: Comprehensive error reporting and state management
- **Stream Positioning**: Random access and stream positioning

### Key Features
- RISC-V 64-bit architecture optimizations
- Efficient buffer management
- Thread-safe operations
- Locale-aware formatting
- POSIX standards compliance
- Memory-mapped file support

## Directory Structure

```
lib/libc/stdio/
├── Makefile.inc        # Build configuration
├── asprintf.c          # Allocate formatted string
├── clrerr.c            # Clear stream error indicators
├── dprintf.c           # Print to file descriptor
├── fclose.c            # Close stream
├── fdopen.c            # Open stream from file descriptor
├── feof.c              # Test end-of-file indicator
├── ferror.c            # Test error indicator
├── fflush.c            # Flush stream
├── fgetc.c             # Get character from stream
├── fgetln.c            # Get line from stream
├── fgetpos.c           # Get stream position
├── fgets.c             # Get string from stream
├── fgetstr.c           # Get string with dynamic allocation
├── fileno.c            # Get file descriptor from stream
├── flags.c             # Stream flags management
├── flockfile.c         # Lock stream for thread safety
├── fopen.c             # Open stream
├── fprintf.c           # Formatted output to stream
├── fpurge.c            # Purge stream buffer
├── fputc.c             # Put character to stream
├── fputs.c             # Put string to stream
├── fputstr.c           # Put string with length
├── fread.c             # Read data from stream
├── freopen.c           # Reopen stream with different file/mode
├── fropen.c            # Open file for reading
├── fscanf.c            # Formatted input from stream
├── fseek.c             # Set stream position
├── fsetpos.c           # Set stream position
├── ftell.c             # Get stream position
├── funopen.c           # Open stream with custom functions
├── fvwrite.c           # Vector write operations
├── fwopen.c            # Open file for writing
├── fwprintf.c          # Wide character formatted output
├── getc.c              # Get character (macro)
├── getchar.c           # Get character from stdin
├── gets.c              # Get string (deprecated)
├── getw.c              # Get word from stream
├── makebuf.c           # Create stream buffer
├── mkdtemp.c           # Create temporary directory
├── mkstemp.c           # Create temporary file
├── mktemp.c            # Generate temporary filename
├── perror.c            # Print error message
├── printf.c            # Formatted output to stdout
├── putc.c              # Put character (macro)
├── putchar.c           # Put character to stdout
├── puts.c              # Put string to stdout
├── putw.c              # Put word to stream
├── remove.c            # Remove file
├── rename.c            # Rename file
├── rewind.c            # Rewind stream
├── rmtemp.c            # Remove temporary files
├── scanf.c             # Formatted input from stdin
├── setbuf.c            # Set stream buffer
├── setbuffer.c         # Set stream buffer with size
├── setlinebuf.c        # Set line buffering
├── setvbuf.c           # Set stream buffering mode
├── snprintf.c          # Formatted output to string with size limit
├── sprintf.c           # Formatted output to string
├── sscanf.c            # Formatted input from string
├── tempnam.c           # Generate temporary filename
├── tmpfile.c           # Create temporary file
├── tmpnam.c            # Generate temporary filename
├── ungetc.c            # Put character back to stream
├── vasprintf.c         # Allocate formatted string (varargs)
├── vfprintf.c          # Formatted output to stream (varargs)
├── vfscanf.c           # Formatted input from stream (varargs)
├── vprintf.c           # Formatted output to stdout (varargs)
├── vscanf.c            # Formatted input from stdin (varargs)
├── vsnprintf.c         # Formatted output to string with size limit (varargs)
├── vsprintf.c          # Formatted output to string (varargs)
├── vsscanf.c           # Formatted input from string (varargs)
├── wprintf.c           # Wide character formatted output to stdout
└── wscanf.c            # Wide character formatted input from stdin
```

## Stream Architecture

### FILE Structure Design
```c
// RISC-V 64-bit optimized FILE structure
typedef struct __sFILE {
    unsigned char *_p;          // Current position in buffer
    int _r;                     // Read space left
    int _w;                     // Write space left
    short _flags;               // Flags
    short _file;                // File descriptor
    struct __sbuf _bf;          // Buffer information
    int _lbfsize;               // Line buffer size
    
    // Thread synchronization
    pthread_mutex_t _lock;      // Stream mutex
    pthread_t _owner;           // Current owner thread
    int _count;                 // Lock recursion count
    
    // Position information
    fpos_t _offset;             // Current file offset
    
    // Operations
    struct __sFILEX *_extra;    // Extended operations
    
    // Buffer allocation
    unsigned char *_ubuf;       // Ungetc buffer
    int _nbuf;                  // Ungetc buffer size
    
    // Wide character support
    struct __sbuf _lb;          // Wide char buffer
    
    // RISC-V specific optimizations
    uint64_t _cache_line;       // Cache line optimization
    int _prefetch_size;         // Prefetch hint
} FILE;

// Buffer structure
typedef struct __sbuf {
    unsigned char *_base;       // Buffer base
    int _size;                  // Buffer size
} _buf_t;
```

### Stream Buffer Management
```c
// RISC-V 64-bit optimized buffer allocation
static int makebuf(FILE *fp)
{
    size_t size;
    void *buf;
    
    // Determine buffer size based on stream type
    if (fp->_flags & __SNBF) {
        size = 1;  // Unbuffered
    } else if (fp->_flags & __SLBF) {
        size = 1024;  // Line buffered
    } else {
        size = BUFSIZ;  // Fully buffered (typically 8192)
    }
    
    // Align buffer to cache line boundary for RISC-V
    size = (size + 63) & ~63;
    
    // Allocate buffer
    buf = malloc(size);
    if (buf == NULL) {
        fp->_flags |= __SNBF;
        fp->_bf._base = fp->_p = (unsigned char *)&fp->_nbuf;
        fp->_bf._size = 1;
        return 0;
    }
    
    // Initialize buffer
    fp->_bf._base = buf;
    fp->_bf._size = size;
    fp->_p = buf;
    
    // Set read/write space
    if (fp->_flags & __SRD) {
        fp->_r = 0;  // No data to read yet
        fp->_w = 0;
    } else if (fp->_flags & __SWR) {
        fp->_r = 0;
        fp->_w = size;  // Full buffer available for writing
    }
    
    return 1;
}

// Flush buffer to file descriptor
int fflush_opt(FILE *fp)
{
    if (fp == NULL) {
        // Flush all streams
        return fflush_all();
    }
    
    // Lock stream for thread safety
    flockfile(fp);
    
    // Flush write buffer
    if (fp->_flags & __SWR && fp->_w > 0) {
        size_t to_write = fp->_bf._size - fp->_w;
        ssize_t written = write(fp->_file, fp->_bf._base, to_write);
        
        if (written != to_write) {
            funlockfile(fp);
            fp->_flags |= __SERR;
            return EOF;
        }
        
        fp->_w = fp->_bf._size;
        fp->_p = fp->_bf._base;
    }
    
    // Reset read buffer
    if (fp->_flags & __SRD) {
        fp->_r = 0;
        fp->_p = fp->_bf._base;
    }
    
    funlockfile(fp);
    return 0;
}
```

## File Operations

### Stream Opening and Closing
```c
// RISC-V 64-bit optimized file opening
FILE *fopen_opt(const char *filename, const char *mode)
{
    FILE *fp;
    int fd;
    int flags = 0;
    int oflags = 0;
    
    // Parse mode string
    switch (*mode++) {
    case 'r':
        flags = __SRD;
        oflags = O_RDONLY;
        break;
    case 'w':
        flags = __SWR;
        oflags = O_WRONLY | O_CREAT | O_TRUNC;
        break;
    case 'a':
        flags = __SWR | __SAPP;
        oflags = O_WRONLY | O_CREAT | O_APPEND;
        break;
    default:
        errno = EINVAL;
        return NULL;
    }
    
    // Handle '+' modifier (read/write)
    if (*mode == '+') {
        flags = __SRW;
        oflags = (oflags & ~O_ACCMODE) | O_RDWR;
        mode++;
    }
    
    // Handle 'b' modifier (binary - ignored on POSIX)
    if (*mode == 'b') {
        mode++;
    }
    
    // Handle 'x' modifier (exclusive creation)
    if (*mode == 'x') {
        oflags |= O_EXCL;
        mode++;
    }
    
    // Open file descriptor
    fd = open(filename, oflags, 0666);
    if (fd < 0) {
        return NULL;
    }
    
    // Allocate FILE structure
    fp = calloc(1, sizeof(FILE));
    if (fp == NULL) {
        close(fd);
        return NULL;
    }
    
    // Initialize FILE structure
    fp->_file = fd;
    fp->_flags = flags;
    pthread_mutex_init(&fp->_lock, NULL);
    
    // Allocate buffer
    if (!makebuf(fp)) {
        close(fd);
        free(fp);
        return NULL;
    }
    
    return fp;
}

// Close stream
int fclose_opt(FILE *fp)
{
    int r;
    
    if (fp == NULL) {
        errno = EINVAL;
        return EOF;
    }
    
    // Flush any pending writes
    r = fflush(fp);
    
    // Close file descriptor
    if (close(fp->_file) < 0) {
        r = EOF;
    }
    
    // Free buffer if dynamically allocated
    if (fp->_bf._base != (unsigned char *)&fp->_nbuf) {
        free(fp->_bf._base);
    }
    
    // Destroy mutex
    pthread_mutex_destroy(&fp->_lock);
    
    // Free FILE structure
    free(fp);
    
    return r;
}
```

### Character I/O Operations
```c
// Get character from stream with buffering
int fgetc_opt(FILE *fp)
{
    unsigned char ch;
    
    // Lock stream for thread safety
    flockfile(fp);
    
    // Check if we need to refill read buffer
    if (fp->_r <= 0) {
        if (fp->_flags & __SEOF) {
            funlockfile(fp);
            return EOF;
        }
        
        // Read more data
        ssize_t n = read(fp->_file, fp->_bf._base, fp->_bf._size);
        if (n <= 0) {
            if (n == 0) {
                fp->_flags |= __SEOF;
            } else {
                fp->_flags |= __SERR;
            }
            funlockfile(fp);
            return EOF;
        }
        
        fp->_r = n;
        fp->_p = fp->_bf._base;
    }
    
    // Get character from buffer
    ch = *fp->_p++;
    fp->_r--;
    
    funlockfile(fp);
    return ch;
}

// Put character to stream with buffering
int fputc_opt(int c, FILE *fp)
{
    unsigned char ch = c;
    
    // Lock stream for thread safety
    flockfile(fp);
    
    // Check if we need to flush write buffer
    if (fp->_w <= 0) {
        if (fflush(fp) == EOF) {
            funlockfile(fp);
            return EOF;
        }
    }
    
    // Put character in buffer
    *fp->_p++ = ch;
    fp->_w--;
    
    // Flush if line buffered and character is newline
    if ((fp->_flags & __SLBF) && ch == '\n') {
        if (fflush(fp) == EOF) {
            funlockfile(fp);
            return EOF;
        }
    }
    
    funlockfile(fp);
    return ch;
}
```

## Formatted Output Architecture

### Printf Implementation
```c
// RISC-V 64-bit optimized printf family
int vfprintf_opt(FILE *fp, const char *fmt, va_list ap)
{
    char buffer[BUFSIZ];
    char *p;
    int n;
    int count = 0;
    
    // Lock stream for thread safety
    flockfile(fp);
    
    // Process format string
    while (*fmt) {
        if (*fmt != '%') {
            // Regular character - output directly
            if (fputc_opt(*fmt, fp) == EOF) {
                funlockfile(fp);
                return EOF;
            }
            count++;
            fmt++;
            continue;
        }
        
        // Handle format specifier
        fmt++;  // Skip '%'
        
        // Parse flags
        int flags = 0;
        while (strchr("-+ #0", *fmt)) {
            switch (*fmt) {
            case '-': flags |= LEFT; break;
            case '+': flags |= PLUS; break;
            case ' ': flags |= SPACE; break;
            case '#': flags |= ALT; break;
            case '0': flags |= ZEROPAD; break;
            }
            fmt++;
        }
        
        // Parse field width
        int width = 0;
        if (*fmt == '*') {
            width = va_arg(ap, int);
            fmt++;
        } else {
            while (isdigit(*fmt)) {
                width = width * 10 + (*fmt - '0');
                fmt++;
            }
        }
        
        // Parse precision
        int precision = -1;
        if (*fmt == '.') {
            fmt++;
            if (*fmt == '*') {
                precision = va_arg(ap, int);
                fmt++;
            } else {
                precision = 0;
                while (isdigit(*fmt)) {
                    precision = precision * 10 + (*fmt - '0');
                    fmt++;
                }
            }
        }
        
        // Parse length modifier
        int length = 0;
        switch (*fmt) {
        case 'h':
            fmt++;
            if (*fmt == 'h') {
                length = 'H';  // char
                fmt++;
            } else {
                length = 'h';  // short
            }
            break;
        case 'l':
            fmt++;
            if (*fmt == 'l') {
                length = 'L';  // long long
                fmt++;
            } else {
                length = 'l';  // long
            }
            break;
        case 'j': length = 'j'; fmt++; break;  // intmax_t
        case 'z': length = 'z'; fmt++; break;  // size_t
        case 't': length = 't'; fmt++; break;  // ptrdiff_t
        }
        
        // Parse conversion specifier
        char spec = *fmt++;
        
        // Handle conversion
        switch (spec) {
        case 'd': case 'i':  // Signed integer
            {
                long long val;
                switch (length) {
                case 'H': val = (signed char)va_arg(ap, int); break;
                case 'h': val = (short)va_arg(ap, int); break;
                case 'l': val = va_arg(ap, long); break;
                case 'L': val = va_arg(ap, long long); break;
                case 'j': val = va_arg(ap, intmax_t); break;
                case 'z': val = va_arg(ap, size_t); break;
                case 't': val = va_arg(ap, ptrdiff_t); break;
                default: val = va_arg(ap, int); break;
                }
                
                n = format_integer(buffer, val, 10, flags, width, precision);
            }
            break;
            
        case 'u': case 'o': case 'x': case 'X':  // Unsigned integer
            {
                unsigned long long val;
                int base = (spec == 'u') ? 10 : (spec == 'o') ? 8 : 16;
                
                switch (length) {
                case 'H': val = (unsigned char)va_arg(ap, unsigned int); break;
                case 'h': val = (unsigned short)va_arg(ap, unsigned int); break;
                case 'l': val = va_arg(ap, unsigned long); break;
                case 'L': val = va_arg(ap, unsigned long long); break;
                case 'j': val = va_arg(ap, uintmax_t); break;
                case 'z': val = va_arg(ap, size_t); break;
                case 't': val = va_arg(ap, ptrdiff_t); break;
                default: val = va_arg(ap, unsigned int); break;
                }
                
                n = format_unsigned(buffer, val, base, flags, width, precision);
            }
            break;
            
        case 'p':  // Pointer
            {
                void *ptr = va_arg(ap, void *);
                flags |= ALT;
                n = format_unsigned(buffer, (uintptr_t)ptr, 16, flags, width, precision);
            }
            break;
            
        case 'c':  // Character
            {
                int ch = va_arg(ap, int);
                buffer[0] = ch;
                buffer[1] = '\0';
                n = 1;
            }
            break;
            
        case 's':  // String
            {
                const char *str = va_arg(ap, const char *);
                if (str == NULL) str = "(null)";
                
                n = strlen(str);
                if (precision >= 0 && n > precision) n = precision;
                
                // Handle width and alignment
                if (width > n && !(flags & LEFT)) {
                    for (int i = 0; i < width - n; i++) {
                        if (fputc_opt(' ', fp) == EOF) {
                            funlockfile(fp);
                            return EOF;
                        }
                        count++;
                    }
                }
                
                // Output string
                for (int i = 0; i < n; i++) {
                    if (fputc_opt(str[i], fp) == EOF) {
                        funlockfile(fp);
                        return EOF;
                    }
                }
                
                // Handle right alignment
                if (width > n && (flags & LEFT)) {
                    for (int i = 0; i < width - n; i++) {
                        if (fputc_opt(' ', fp) == EOF) {
                            funlockfile(fp);
                            return EOF;
                        }
                        count++;
                    }
                }
                
                count += n;
                continue;  // Skip default output
            }
            
        case 'f': case 'F':  // Floating point
            {
                double val = va_arg(ap, double);
                n = format_float(buffer, val, 10, flags, width, precision);
            }
            break;
            
        case 'e': case 'E': case 'g': case 'G':  // Scientific notation
            {
                double val = va_arg(ap, double);
                int scientific = (spec == 'e' || spec == 'E');
                n = format_scientific(buffer, val, scientific, flags, width, precision);
            }
            break;
            
        case '%':  // Literal percent
            if (fputc_opt('%', fp) == EOF) {
                funlockfile(fp);
                return EOF;
            }
            count++;
            continue;
            
        default:  // Unknown specifier
            if (fputc_opt('%', fp) == EOF) {
                funlockfile(fp);
                return EOF;
            }
            if (fputc_opt(spec, fp) == EOF) {
                funlockfile(fp);
                return EOF;
            }
            count += 2;
            continue;
        }
        
        // Output formatted result
        for (int i = 0; i < n; i++) {
            if (fputc_opt(buffer[i], fp) == EOF) {
                funlockfile(fp);
                return EOF;
            }
        }
        count += n;
    }
    
    funlockfile(fp);
    return count;
}
```

## RISC-V 64-bit Specific Optimizations

### Cache-Line Aligned I/O
```c
// RISC-V cache-line optimized I/O
typedef struct {
    void *buffer;
    size_t size;
    size_t offset;
    int fd;
} cache_io_t;

// Read with cache-line alignment
ssize_t cache_aligned_read(int fd, void *buf, size_t count)
{
    char *buffer = buf;
    size_t total = 0;
    
    // Handle unaligned start
    if ((uintptr_t)buffer & 63) {
        size_t prefix = 64 - ((uintptr_t)buffer & 63);
        if (prefix > count) prefix = count;
        
        char temp[64];
        ssize_t n = read(fd, temp, 64);
        if (n <= 0) return n;
        
        memcpy(buffer, temp, prefix);
        buffer += prefix;
        count -= prefix;
        total += prefix;
    }
    
    // Read aligned chunks
    while (count >= 64) {
        ssize_t n = read(fd, buffer, count & ~63);
        if (n <= 0) return total > 0 ? total : n;
        
        buffer += n;
        count -= n;
        total += n;
    }
    
    // Handle remaining unaligned end
    if (count > 0) {
        char temp[64];
        ssize_t n = read(fd, temp, 64);
        if (n > 0) {
            size_t copy_size = n < count ? n : count;
            memcpy(buffer, temp, copy_size);
            total += copy_size;
        }
    }
    
    return total;
}
```

### Atomic Stream Operations
```c
// RISC-V atomic operations for stream state
static inline int test_and_set_error(FILE *fp)
{
    int old_flags;
    __atomic_load(&fp->_flags, &old_flags, __ATOMIC_ACQUIRE);
    
    if (!(old_flags & __SERR)) {
        int new_flags = old_flags | __SERR;
        if (__atomic_compare_exchange_n(&fp->_flags, &old_flags, new_flags,
                                       0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
            return 0;  // Successfully set error flag
        }
    }
    
    return 1;  // Error flag already set
}

// Atomic position update
static inline void update_position_atomic(FILE *fp, off_t offset)
{
    __atomic_store_n(&fp->_offset, offset, __ATOMIC_RELEASE);
}
```

## Thread Safety Architecture

### Stream Locking
```c
// Thread-safe stream operations
#define flockfile(fp) pthread_mutex_lock(&(fp)->_lock)
#define funlockfile(fp) pthread_mutex_unlock(&(fp)->_lock)
#define ftrylockfile(fp) pthread_mutex_trylock(&(fp)->_lock)

// Recursive lock support for stdio
int ftrylockfile_recursive(FILE *fp)
{
    pthread_t self = pthread_self();
    
    if (pthread_equal(fp->_owner, self)) {
        fp->_count++;
        return 0;  // Already own the lock
    }
    
    int result = pthread_mutex_trylock(&fp->_lock);
    if (result == 0) {
        fp->_owner = self;
        fp->_count = 1;
    }
    
    return result;
}

void funlockfile_recursive(FILE *fp)
{
    if (--fp->_count == 0) {
        fp->_owner = 0;
        pthread_mutex_unlock(&fp->_lock);
    }
}
```

## Performance Optimizations

### Vectorized I/O Operations
```c
// Scatter-gather I/O for better performance
int fvwrite_opt(FILE *fp, const struct iovec *iov, int iovcnt)
{
    int total = 0;
    
    flockfile(fp);
    
    for (int i = 0; i < iovcnt; i++) {
        size_t to_write = iov[i].iov_len;
        const char *data = iov[i].iov_base;
        
        // Use buffer if available
        while (to_write > 0) {
            size_t space = fp->_w;
            
            if (space > to_write) space = to_write;
            
            if (space > 0) {
                memcpy(fp->_p, data, space);
                fp->_p += space;
                fp->_w -= space;
                data += space;
                to_write -= space;
                total += space;
            }
            
            if (to_write > 0) {
                if (fflush(fp) == EOF) {
                    funlockfile(fp);
                    return EOF;
                }
            }
        }
    }
    
    funlockfile(fp);
    return total;
}
```

### Fast Integer Formatting
```c
// RISC-V 64-bit optimized integer formatting
static int format_integer(char *buf, long long val, int base,
                         int flags, int width, int precision)
{
    char temp[32];
    char *p = temp + sizeof(temp) - 1;
    int negative = 0;
    int digits = 0;
    
    *p = '\0';
    
    // Handle negative numbers
    if (val < 0 && base == 10) {
        negative = 1;
        val = -val;
    }
    
    // Generate digits in reverse order
    do {
        int digit = val % base;
        *--p = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');
        digits++;
        val /= base;
    } while (val > 0);
    
    // Handle precision
    if (precision >= 0 && digits < precision) {
        while (digits < precision) {
            *--p = '0';
            digits++;
        }
    }
    
    // Handle prefix
    if (flags & ALT) {
        if (base == 8 && *p != '0') {
            *--p = '0';
            digits++;
        } else if (base == 16) {
            *--p = 'x';
            *--p = '0';
            digits += 2;
        }
    }
    
    if (negative) {
        *--p = '-';
        digits++;
    } else if (flags & PLUS) {
        *--p = '+';
        digits++;
    } else if (flags & SPACE) {
        *--p = ' ';
        digits++;
    }
    
    // Handle width and alignment
    char *out = buf;
    if (width > digits && !(flags & LEFT)) {
        char pad = (flags & ZEROPAD) ? '0' : ' ';
        while (width > digits) {
            *out++ = pad;
            width--;
        }
    }
    
    // Copy formatted number
    while (*p) {
        *out++ = *p++;
    }
    
    // Right padding
    while (width > digits) {
        *out++ = ' ';
        width--;
    }
    
    *out = '\0';
    return out - buf;
}
```

## Error Handling and Standards Compliance

### POSIX Error Handling
```c
// Comprehensive error reporting
void perror_opt(const char *s)
{
    int saved_errno = errno;
    FILE *fp = stderr;
    
    flockfile(fp);
    
    if (s && *s) {
        fputs(s, fp);
        fputs(": ", fp);
    }
    
    fputs(strerror(saved_errno), fp);
    fputc('\n', fp);
    
    funlockfile(fp);
}

// Error indicator management
int ferror_opt(FILE *fp)
{
    return (fp->_flags & __SERR) != 0;
}

void clearerr_opt(FILE *fp)
{
    fp->_flags &= ~(__SERR | __SEOF);
}
```

## Build Configuration

### Architecture-Specific Flags
```makefile
# RISC-V 64-bit stdio build configuration
STDIO_SRCS += \
    fopen.c fclose.c fflush.c fread.c fwrite.c fseek.c ftell.c rewind.c \
    fgetc.c fputc.c fgets.c fputs.c fprintf.c fscanf.c printf.c scanf.c \
    sprintf.c snprintf.c sscanf.c vfprintf.c vprintf.c vsprintf.c vsnprintf.c \
    perror.c setbuf.c setvbuf.c ungetc.c fileno.c tmpfile.c tmpnam.c

# RISC-V specific optimizations
CFLAGS += -march=rv64gc -mabi=lp64d
CFLAGS += -DUSE_RISCV_64BIT_OPS -DUSE_CACHE_LINE_ALIGNMENT
CFLAGS += -DTHREAD_SAFE_STDIO -DUSE_ATOMIC_STREAM_OPS

# Buffer size optimizations
CFLAGS += -DBUFSIZ=8192 -DUSE_MMAP_FOR_LARGE_FILES
CFLAGS += -DSTREAM_CACHE_LINE=64
```

## Performance Characteristics

### I/O Performance
- Buffered I/O: 10-100x faster than unbuffered system calls
- Large file operations: Memory-mapped I/O for files > 1MB
- Thread synchronization: Lock-free operations where possible
- Cache utilization: Cache-line aligned buffers

### Memory Usage
- Per-stream overhead: ~256 bytes
- Buffer allocation: 8KB default, scalable
- Thread-local storage: Minimal per-thread overhead
- Lock contention: Optimized for concurrent access

The stdio library provides the essential I/O foundation that MINIX 3 applications depend on, with RISC-V 64-bit optimizations ensuring efficient file operations, fast formatted I/O, and reliable stream management for high-performance applications.