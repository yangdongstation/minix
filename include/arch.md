# MINIX Standard Include Headers Architecture

This directory contains the standard C library headers and system interface definitions that provide the POSIX and ANSI C API for MINIX applications. These headers are compatible with NetBSD and provide a standard programming interface.

## 本仓库实际包含/可选或上游组件

本仓库实际包含：以下为顶层条目示例（完整清单以目录实际内容为准）：`arpa/`, `protocols/`, `rpc/`, `rpcsvc/`, `ssp/`, `stdio.h`, `unistd.h`.
可选或上游组件：上文条目可能包含上游/可选组件示例，当前仓库未必包含全部。

## Standard C Library Headers

### Basic Language Support
- **stddef.h**: Fundamental types and macros (size_t, NULL, offsetof)
- **limits.h**: Implementation limits (INT_MAX, CHAR_BIT, etc.)
- **stdint.h**: Fixed-width integer types (int32_t, uint64_t, etc.)
- **stdbool.h**: Boolean type and values (bool, true, false)
- **iso646.h**: Alternative spellings (and, or, not, etc.)

### Memory Management
- **stdlib.h**: General utilities (malloc, free, atoi, exit)
- **string.h**: String operations (strcpy, strcat, strlen)
- **strings.h**: Additional string functions (bcopy, bzero)
- **memory.h**: Memory operations (memcpy, memset)
- **malloc.h**: Memory allocation debugging

### Input/Output
- **stdio.h**: Standard I/O (printf, scanf, fopen, fread)
- **fcntl.h**: File control (open, fcntl)
- **unistd.h**: POSIX system calls (read, write, close)
- **dirent.h**: Directory operations (opendir, readdir)

### Mathematical Functions
- **math.h**: Mathematical functions (sin, cos, sqrt, pow)
- **tgmath.h**: Type-generic mathematical macros
- **complex.h**: Complex number support
- **fenv.h**: Floating-point environment
- **ieeefp.h**: IEEE floating-point definitions

### Localization
- **locale.h**: Localization support (setlocale, localeconv)
- **langinfo.h**: Language information (nl_langinfo)
- **monetary.h**: Monetary formatting (strfmon)
- **wchar.h**: Wide character support
- **wctype.h**: Wide character classification

### Error Handling
- **errno.h**: Error numbers (errno, perror)
- **assert.h**: Assertion macros (assert)
- **err.h**: Error reporting functions (err, warn)
- **sysexits.h**: Exit status codes (EX_OK, EX_USAGE)

## POSIX System Interface Headers

### Process Management
- **signal.h**: Signal handling (signal, kill, sigaction)
- **spawn.h**: Process spawning (posix_spawn)
- **sched.h**: Process scheduling (sched_setscheduler)
- **setjmp.h**: Non-local jumps (setjmp, longjmp)
- **ucontext.h**: User context (getcontext, setcontext)

### File System Operations
- **stat.h**: File status (stat, chmod, mkdir)
- **fstab.h**: File system table (/etc/fstab)
- **mntopts.h**: Mount options parsing
- **ftw.h**: File tree walk (ftw, nftw)
- **fts.h**: File traversal (fts_open, fts_read)
- **glob.h**: Pathname pattern matching (glob)
- **fnmatch.h**: Filename matching (fnmatch)

### Directory Operations
- **pwd.h**: Password file operations (getpwnam)
- **grp.h**: Group file operations (getgrnam)
- **utmp.h/utmpx.h**: User accounting (utmp, wtmp)
- **ttyent.h**: Terminal information (/etc/ttys)

### Inter-Process Communication
- **mqueue.h**: Message queues (mq_open, mq_send)
- **semaphore.h**: POSIX semaphores (sem_open)
- **sys/ipc.h**: System V IPC (ftok)
- **sys/msg.h**: System V message queues
- **sys/sem.h**: System V semaphores
- **sys/shm.h**: System V shared memory

### Network Programming
- **socket.h**: Socket interface (socket, bind, connect)
- **netdb.h**: Network database (gethostbyname, getservbyname)
- **netinet/in.h**: Internet address structures
- **arpa/inet.h**: Internet address conversion
- **ifaddrs.h**: Interface address structures
- **net/if.h**: Network interface structures

### Time and Date
- **time.h**: Time functions (time, localtime, strftime)
- **tzfile.h**: Timezone file format
- **utime.h**: File timestamps (utime)

## MINIX-Specific Headers

### MINIX System Interface
- **minix/**: MINIX-specific system calls and structures
- **minix/config.h**: MINIX configuration options
- **minix/const.h**: MINIX constants and limits
- **minix/type.h**: MINIX-specific types

### MINIX IPC
- **minix/ipc.h**: Inter-process communication
- **minix/com.h**: System call message definitions
- **minix/callnr.h**: System call numbers

### MINIX Drivers
- **minix/driver.h**: Driver interface definitions
- **minix/chardriver.h**: Character driver framework
- **minix/blockdriver.h**: Block driver framework
- **minix/partition.h**: Partition table definitions

## Data Structure Headers

### Dynamic Data Structures
- **queue.h**: Circular queues and lists
- **tree.h**: Red-black trees
- **hash.h**: Hash tables
- **bitstring.h**: Bit string operations

### Database Operations
- **db.h**: Berkeley DB interface
- **ndbm.h**: Network database manager
- **mpool.h**: Memory pool management

## Security and Cryptography

### Security Headers
- **ssp/**: Stack-Smashing Protection
- **md2.h**: MD2 hash algorithm
- **paths.h**: Standard file paths (/etc/passwd, etc.)

### Authentication
- **login_cap.h**: Login capabilities
- **bsd_auth.h**: BSD authentication

## Utility Headers

### Regular Expressions
- **regex.h**: POSIX regular expressions (regcomp, regexec)
- **regexp.h**: BSD regular expressions
- **re_comp.h**: Regular expression compilation

### Pattern Matching
- **glob.h**: Globbing patterns (glob)
- **wordexp.h**: Word expansion (wordexp)

### String Processing
- **vis.h**: String visibility encoding
- **nlist.h**: Name list operations (nlist)
- **search.h**: Search tables (lsearch, hsearch)

### Internationalization
- **iconv.h**: Character set conversion (iconv)
- **gettext.h**: Message translation (gettext)

## Network Protocol Headers

### Internet Protocols
- **protocols/**: IP protocol numbers
- **rpcsvc/**: RPC service definitions
- **rpc/**: Remote Procedure Call definitions
- **resolv.h**: DNS resolver interface

### Application Protocols
- **arpa/**: ARPA standard protocols
- **netgroup.h**: Network group definitions
- **nsswitch.h**: Name service switch

## File Format Headers

### Archive Formats
- **ar.h**: Archive format (ar archives)
- **tar.h**: Tar archive format
- **cpio.h**: CPIO archive format

### Executable Formats
- **a.out.h**: a.out executable format
- **link.h**: Dynamic linking interface
- **link_elf.h**: ELF linking definitions
- **link_aout.h**: a.out linking definitions
- **stab.h**: Symbol table definitions

### Configuration Files
- **disktab.h**: Disk parameter table
- **fstab.h**: File system table

## Build System Integration

### Makefile Structure
- **Makefile**: Header installation configuration
- Platform-specific header selection
- Conditional header installation
- Cross-compilation support

### Header Dependencies
- **Cross-references**: Inter-header dependencies
- **Feature detection**: Compile-time header availability
- **Architecture support**: Platform-specific headers
- **Version compatibility**: API version management

## Development Guidelines

### Header Design Principles
- **Standards compliance**: POSIX and ANSI C compatibility
- **Namespace cleanliness**: Proper symbol naming
- **Documentation**: Comprehensive header comments
- **Portability**: Cross-platform compatibility

### MINIX Integration
- **System call interface**: MINIX-specific extensions
- **Message passing**: IPC-related definitions
- **Microkernel support**: Userspace server interfaces
- **Driver framework**: Device driver definitions

This comprehensive header collection provides a complete POSIX-compatible programming environment while maintaining MINIX-specific functionality for the microkernel architecture, with full support for the RISC-V 64-bit architecture port.
