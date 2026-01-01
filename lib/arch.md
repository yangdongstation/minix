# MINIX System Libraries Architecture

This directory contains the standard C library and support libraries that provide the runtime environment for MINIX applications. These libraries are synced with NetBSD-8 and provide comprehensive POSIX and ANSI C functionality.

## 本仓库实际包含/可选或上游组件

本仓库实际包含：以下为顶层条目示例（完整清单以目录实际内容为准）：`libc/`, `libm/`, `libutil/`, `libz/`, `lua/`, `Makefile`.
可选或上游组件：上文条目可能包含上游/可选组件示例，当前仓库未必包含全部。

## Core Runtime Libraries

### Standard C Library (libc/)
The primary C library providing core language support:
- **gen/**: Generic C library functions
  - String manipulation (strcpy, strcat, strlen)
  - Memory management (malloc, free, realloc)
  - I/O operations (fopen, fread, fprintf)
  - Mathematical functions (sin, cos, sqrt)
  - Time functions (time, localtime, strftime)
  - Process control (fork, exec, wait)
  - File operations (open, read, write, close)

- **arch/**: Architecture-specific implementations
  - **riscv64/**: RISC-V 64-bit specific code
  - Assembly language optimizations
  - Platform-specific system calls
  - Context switching support
  - Atomic operations

- **atomic/**: Atomic operation support
  - Lock-free programming primitives
  - Memory barriers
  - Atomic read-modify-write operations

- **include/**: Library internal headers
- **compat/**: Compatibility layers
- **citrus/**: Character set conversion
- **gdtoa/**: Floating-point conversion
- **hash/**: Hash table implementation
- **inet/**: Internet address support
- **db/**: Database functions (dbm)
- **dlfcn/**: Dynamic linking support
- **iconv/**: Character encoding conversion
- **isc/**: ISC compatibility functions
- **locale/**: Localization support
- **net/**: Network database functions
- **locale/**: Internationalization support
- **misc/**: Miscellaneous functions
- **mp/**: Multiple precision arithmetic
- **multibyte/**: Multi-byte character support
- **nfs/**: Network File System support
- **quad/**: 64-bit arithmetic
- **regex/**: Regular expression support
- **rpc/**: Remote Procedure Call support
- **stdio/**: Standard I/O implementation
- **stdlib/**: Standard library functions
- **string/**: String manipulation
- **sys/**: System call wrappers
- **termios/**: Terminal I/O
- **time/**: Time and date functions
- **uuid/**: UUID generation

### Math Library (libm/)
Mathematical function library:
- Trigonometric functions (sin, cos, tan)
- Exponential and logarithmic functions (exp, log)
- Power functions (pow, sqrt)
- Hyperbolic functions (sinh, cosh)
- Rounding functions (floor, ceil)
- Special functions (gamma, erf)

### Curses Library (libcurses/)
Terminal control library:
- Screen management
- Window operations
- Input handling
- Color support
- Panel library
- Form library (libform)
- Menu library (libmenu)
- Terminal information (libterminfo)

## Security Libraries

### Cryptography (libcrypt/)
Password hashing and encryption:
- Traditional crypt()
- MD5 password hashing
- SHA password hashing
- Blowfish password hashing

### TCP Wrappers (libwrap/)
Network access control:
- Host access control
- Service access control
- Logging support

## Development Libraries

### Execution Info (libexecinfo/)
Stack trace support:
- Backtrace generation
- Symbol resolution
- Debug information

### Kernel VM (libkvm/)
Kernel memory access:
- Kernel memory reading
- Process inspection
- System debugging

### Edit Library (libedit/)
Command line editing:
- Readline compatibility
- History management
- Tab completion

## Compression Libraries

### zlib (libz/)
Data compression:
- Deflate compression
- gzip format support
- Memory compression

### Bzip2 (libbz2/)
High-quality compression:
- bzip2 format
- Block sorting compression

## Protocol Libraries

### RPC (librpcsvc/)
Remote Procedure Call support:
- Sun RPC implementation
- Portmapper support
- RPC service definitions

### Puffs (libpuffs/)
Pass-to-Userspace Framework:
- Userspace file systems
- Protocol translation
- File system in userspace

### Refuse (librefuse/)
FUSE compatibility layer:
- FUSE interface
- Userspace file systems
- Linux compatibility

### Telnet (libtelnet/)
Telnet protocol support:
- Terminal negotiation
- Protocol handling
- Option processing

## Internationalization

### libintl
Internationalization support:
- Message translation
- Locale management
- gettext() implementation

### i18n Module (i18n_module/)
Character set modules:
- Character encoding support
- Locale data
- Cultural conventions

## Lua Support

### Lua Library (lua/)
Scripting language support:
- Lua interpreter integration
- Embedded scripting
- Configuration management

## Version Management

### Version Checking Scripts
- **bumpversion**: Version number management
- **checkver**: Version consistency checking
- **checkvers**: Multi-component version validation
- **checkoldver**: Legacy version compatibility

## Build System

### Makefile Structure
- **Makefile**: Top-level library build configuration
- **Makefile.inc**: Common build definitions
- Architecture-specific build rules
- Cross-compilation support

### Library Dependencies
- **Dependency tracking**: Inter-library dependencies
- **Link order**: Proper library linking sequence
- **Version management**: Library versioning support
- **Symbol resolution**: Export/import management

## Architecture Support

### RISC-V 64-bit Support
- **arch/riscv64/**: RISC-V 64-bit specific implementations
- Assembly optimizations for RISC-V
- Platform-specific system call interfaces
- Context switching primitives
- Atomic operation implementations
- Memory barrier operations

### Multi-Architecture Support
The libraries support multiple architectures:
- ARM (32-bit and 64-bit)
- Intel x86 (32-bit and 64-bit)
- MIPS (32-bit and 64-bit)
- PowerPC (32-bit and 64-bit)
- SPARC (32-bit and 64-bit)
- RISC-V (32-bit and 64-bit)
- And many others

## Runtime Features

### Thread Safety
- Thread-safe functions
- Reentrant interfaces
- Mutex protection where needed
- Atomic operations

### Error Handling
- Comprehensive error reporting
- errno support
- Error code translation
- Debugging support

### Performance Optimizations
- Assembly language optimizations
- Platform-specific fast paths
- Memory allocation optimization
- I/O buffering optimization

## Standards Compliance

### POSIX Compliance
Full POSIX.1-2008 support where applicable:
- System interfaces
- Shell and utilities
- Threads
- Real-time extensions

### ANSI C Compliance
Complete ANSI C (C89/C90) support:
- Language facilities
- Library functions
- Headers and definitions

### BSD Extensions
NetBSD-compatible extensions:
- Additional functions
- Enhanced features
- Compatibility interfaces

This comprehensive library collection provides the runtime foundation for MINIX applications, ensuring compatibility with existing software while providing optimized implementations for the RISC-V 64-bit architecture and maintaining the microkernel design philosophy.
