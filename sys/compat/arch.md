# MINIX System Compatibility Layer

## Overview

The `sys/compat/` directory implements the system compatibility layer in MINIX 3, providing essential interfaces for maintaining compatibility between different system components, architectures, and external software requirements. This directory serves as the bridge between MINIX's microkernel architecture and traditional UNIX/POSIX interfaces, ensuring seamless integration with existing software ecosystems.

## Purpose and Responsibilities

### Core Functions
- **API Compatibility**: Provides compatibility interfaces for different system APIs
- **Binary Compatibility**: Ensures binary compatibility across system versions
- **Architecture Abstraction**: Handles architecture-specific compatibility requirements
- **Standards Compliance**: Maintains compliance with POSIX and other standards
- **Legacy Support**: Provides interfaces for legacy system calls and libraries

### Key Features
- **System Call Translation** - Converts between different system call interfaces
- **Data Structure Mapping** - Maps between different data structure formats
- **Error Code Translation** - Translates error codes between systems
- **Signal Handling Compatibility** - Maintains signal handling compatibility
- **File System Interface Compatibility** - Provides consistent file system interfaces

## Directory Structure

```
sys/compat/
├── Makefile               # Main compatibility makefile
└── sys/                   # System compatibility interfaces
    ├── Makefile           # System compatibility makefile
    ├── _errno.h           # Error number compatibility
    ├── _select.h          # Select system call compatibility
    ├── _siginfo.h         # Signal information compatibility
    ├── _sigset.h          # Signal set compatibility
    ├── _termios.h         # Terminal I/O compatibility
    ├── _timeval.h         # Time value compatibility
    ├── _types.h           # Type definitions compatibility
    ├── arch.h             # Architecture compatibility
    ├── audioio.h          # Audio I/O compatibility
    ├── btio.h             # Bluetooth I/O compatibility
    ├── cdefs.h            # C definitions compatibility
    ├── disklabel.h        # Disk label compatibility
    ├── endian.h           # Endianness compatibility
    ├── errno.h            # Error number compatibility
    ├── errno2.h           # Extended error compatibility
    ├── exec.h             # Execution compatibility
    ├── featuretest.h      # Feature test compatibility
    ├── fftypes.h          # File system types compatibility
    ├── file.h             # File operations compatibility
    ├── ioccom.h           # I/O control compatibility
    ├── ioctl.h            # I/O control compatibility
    ├── ipc.h              /* Inter-process communication compatibility
    ├── ioctl_compat.h     # I/O control compatibility layer
    ├── kernel.h           # Kernel interface compatibility
    ├── ksem.h             # Kernel semaphore compatibility
    ├── lock.h             # Locking compatibility
    ├── malloc.h           # Memory allocation compatibility
    ├── mman.h             # Memory management compatibility
    ├── module.h           # Module compatibility
    ├── mount.h            # Mount operations compatibility
    ├── msg.h              # Message queue compatibility
    ├── mutex.h            # Mutex compatibility
    ├── param.h            # System parameters compatibility
    ├── pciio.h            # PCI I/O compatibility
    ├── poll.h             # Poll compatibility
    ├── proc.h             # Process compatibility
    ├── procfs.h           # Process file system compatibility
    ├── pthread.h          # POSIX threads compatibility
    ├── ptrace.h           # Process tracing compatibility
    ├── queue.h            # Queue compatibility
    ├── resource.h         # Resource limits compatibility
    ├── rtprio.h           # Real-time priority compatibility
    ├── sched.h            # Scheduling compatibility
    ├── select.h           # Select compatibility
    ├── semaphore.h        # Semaphore compatibility
    ├── signal.h           # Signal handling compatibility
    ├── signalvar.h        # Signal variable compatibility
    ├── socket.h           # Socket compatibility
    ├── sockio.h           # Socket I/O compatibility
    ├── stat.h             # File status compatibility
    ├── stddef.h           # Standard definitions compatibility
    ├── stdint.h           # Standard integer compatibility
    ├── stdio.h            # Standard I/O compatibility
    ├── stdlib.h           # Standard library compatibility
    ├── stropts.h          # Stream I/O compatibility
    ├── systm.h            # System compatibility
    ├── termios.h          # Terminal I/O compatibility
    ├── time.h             # Time compatibility
    ├── timespec.h         # Time specification compatibility
    ├── timex.h            # Time extension compatibility
    ├── types.h            # Type definitions compatibility
    ├── ucontext.h         # User context compatibility
    ├── uio.h              # User I/O compatibility
    ├── un.h               /* Unix domain socket compatibility
    ├── unistd.h           # Unix standard compatibility
    ├── user.h             # User structure compatibility
    ├── utsname.h          # Unix name compatibility
    ├── vfs_syscalls.h     # VFS system calls compatibility
    ├── vnode.h            # Virtual node compatibility
    ├── wait.h             # Process wait compatibility
    └── xattr.h            # Extended attributes compatibility
```

## Compatibility Layer Architecture

### System Call Compatibility Framework
The compatibility layer implements a comprehensive framework for system call translation:

```c
/* System call compatibility structure */
struct compat_sysent {
    int     (*sy_call)(void *);     /* Compatibility system call handler */
    int     sy_narg;                /* Number of arguments */
    int     sy_flags;               /* System call flags */
    const char *sy_name;            /* System call name */
};

/* Error code translation table */
struct compat_errno_map {
    int     native_errno;           /* Native error number */
    int     compat_errno;           /* Compatibility error number */
    const char *compat_name;        /* Compatibility error name */
};
```

### Data Structure Compatibility
The layer provides mapping between different data structure formats:

```c
/* Compatibility time value structure */
struct compat_timeval {
    int32_t     tv_sec;             /* Seconds (32-bit) */
    int32_t     tv_usec;            /* Microseconds (32-bit) */
};

/* Compatibility signal set */
typedef uint32_t    compat_sigset_t;

/* Compatibility file status */
struct compat_stat {
    uint32_t    st_dev;             /* Device ID */
    uint32_t    st_ino;             /* Inode number */
    uint16_t    st_mode;            /* File mode */
    uint16_t    st_nlink;           /* Number of links */
    uint32_t    st_uid;             /* User ID */
    uint32_t    st_gid;             /* Group ID */
    uint32_t    st_rdev;            /* Raw device ID */
    int32_t     st_size;            /* File size */
    int32_t     st_atime;           /* Access time */
    int32_t     st_mtime;           /* Modification time */
    int32_t     st_ctime;           /* Change time */
};
```

## MINIX-Specific Compatibility Features

### Microkernel Compatibility Interface
The compatibility layer bridges MINIX's microkernel architecture with traditional monolithic kernel interfaces:

```c
/* MINIX to POSIX system call translation */
static struct compat_sysent minix_posix_sysent[] = {
    { compat_read, 3, 0, "read" },      /* MINIX message-based read */
    { compat_write, 3, 0, "write" },    /* MINIX message-based write */
    { compat_open, 3, 0, "open" },      /* MINIX VFS-based open */
    { compat_close, 1, 0, "close" },    /* MINIX message-based close */
    /* ... additional system calls ... */
};

/* Message-passing to system call translation */
int compat_msg_to_syscall(const message *msg, int *syscall_ret) {
    int syscall_nr = msg->m_type;
    
    switch (syscall_nr) {
    case VFS_READ:
        return compat_do_read(msg, syscall_ret);
    case VFS_WRITE:
        return compat_do_write(msg, syscall_ret);
    /* ... additional translations ... */
    default:
        return ENOSYS;
    }
}
```

### Architecture Compatibility Layer
Provides architecture-independent interfaces for architecture-specific operations:

```c
/* Architecture-neutral system calls */
#ifdef __i386__
#include "compat/arch/i386.h"
#elif defined(__arm__)
#include "compat/arch/arm.h"
#elif defined(__riscv64__)
#include "compat/arch/riscv64.h"
#endif

/* Generic architecture interface */
int compat_arch_set_thread_area(void *tls_base) {
    return arch_set_thread_area(tls_base);
}

int compat_arch_get_thread_area(void *tls_base) {
    return arch_get_thread_area(tls_base);
}
```

## Error Handling Compatibility

### Error Code Translation
Comprehensive error code mapping between different systems:

```c
/* MINIX to POSIX error code mapping */
static const struct compat_errno_map minix_posix_errno_map[] = {
    { EPERM,   EPERM,   "EPERM" },      /* Operation not permitted */
    { ENOENT,  ENOENT,  "ENOENT" },     /* No such file or directory */
    { ESRCH,   ESRCH,   "ESRCH" },      /* No such process */
    { EINTR,   EINTR,   "EINTR" },      /* Interrupted system call */
    { EIO,     EIO,     "EIO" },        /* I/O error */
    { ENXIO,   ENXIO,   "ENXIO" },      /* No such device or address */
    { E2BIG,   E2BIG,   "E2BIG" },      /* Argument list too long */
    { ENOEXEC, ENOEXEC, "ENOEXEC" },    /* Exec format error */
    { EBADF,   EBADF,   "EBADF" },      /* Bad file number */
    /* ... additional error mappings ... */
};

/* Error code translation function */
int compat_translate_errno(int native_errno) {
    for (int i = 0; i < nitems(minix_posix_errno_map); i++) {
        if (minix_posix_errno_map[i].native_errno == native_errno) {
            return minix_posix_errno_map[i].compat_errno;
        }
    }
    return EINVAL;  /* Default to generic error */
}
```

### Signal Compatibility
Signal handling compatibility between different signal models:

```c
/* Signal number compatibility mapping */
static const int compat_signal_map[] = {
    /* POSIX signals */
    [SIGINT]    = SIGINT,     /* Interrupt */
    [SIGILL]    = SIGILL,     /* Illegal instruction */
    [SIGABRT]   = SIGABRT,    /* Abort */
    [SIGFPE]    = SIGFPE,     /* Floating point exception */
    [SIGSEGV]   = SIGSEGV,    /* Segmentation fault */
    [SIGTERM]   = SIGTERM,    /* Termination */
    /* ... additional signal mappings ... */
};

/* Signal action compatibility */
struct compat_sigaction {
    uint32_t    sa_handler;     /* Signal handler */
    uint32_t    sa_flags;       /* Signal flags */
    compat_sigset_t sa_mask;    /* Signal mask */
};
```

## File System Compatibility

### File System Interface Compatibility
Provides consistent interfaces across different file system implementations:

```c
/* File system compatibility layer */
struct compat_statvfs {
    uint32_t    f_bsize;        /* File system block size */
    uint32_t    f_frsize;       /* Fragment size */
    uint64_t    f_blocks;       /* Size of fs in f_frsize units */
    uint64_t    f_bfree;        /* Number of free blocks */
    uint64_t    f_bavail;       /* Number of free blocks for unprivileged users */
    uint64_t    f_files;        /* Number of inodes */
    uint64_t    f_ffree;        /* Number of free inodes */
    uint64_t    f_favail;       /* Number of free inodes for unprivileged users */
    uint32_t    f_fsid;         /* File system ID */
    uint32_t    f_flag;         /* Mount flags */
    uint32_t    f_namemax;      /* Maximum filename length */
};

/* File system operation translation */
int compat_statvfs(const char *path, struct compat_statvfs *buf) {
    struct statvfs native_buf;
    int ret = statvfs(path, &native_buf);
    
    if (ret == 0) {
        /* Translate to compatibility format */
        buf->f_bsize = native_buf.f_bsize;
        buf->f_frsize = native_buf.f_frsize;
        buf->f_blocks = native_buf.f_blocks;
        /* ... additional field translations ... */
    }
    
    return ret;
}
```

### Path Name Compatibility
Handles differences in path name conventions and limitations:

```c
/* Path name translation utilities */
int compat_translate_path(const char *compat_path, char *native_path, size_t size) {
    /* Handle path prefix translations */
    if (strncmp(compat_path, "/compat/", 8) == 0) {
        /* Translate compatibility paths */
        snprintf(native_path, size, "%s", compat_path + 8);
    } else {
        /* Pass through native paths */
        strlcpy(native_path, compat_path, size);
    }
    return 0;
}
```

## Network and Socket Compatibility

### Socket Interface Compatibility
Maintains compatibility across different socket implementations:

```c
/* Socket address compatibility */
struct compat_sockaddr {
    uint8_t     sa_len;         /* Length of address */
    uint8_t     sa_family;      /* Address family */
    char        sa_data[14];    /* Address data */
};

/* Socket option compatibility */
struct compat_sockopt {
    int         level;          /* Option level */
    int         name;           /* Option name */
    void        *val;           /* Option value */
    socklen_t   len;            /* Option length */
};

/* Socket operation translation */
int compat_socket(int domain, int type, int protocol) {
    /* Translate socket parameters */
    int native_domain = translate_domain(domain);
    int native_type = translate_type(type);
    
    return socket(native_domain, native_type, protocol);
}
```

### Network Protocol Compatibility
Ensures compatibility across different network protocol implementations:

```c
/* Network protocol compatibility */
struct compat_in_addr {
    uint32_t    s_addr;         /* Network byte order address */
};

struct compat_sockaddr_in {
    uint8_t     sin_len;        /* Length */
    uint8_t     sin_family;     /* AF_INET */
    uint16_t    sin_port;       /* Port number */
    struct compat_in_addr sin_addr; /* IP address */
    char        sin_zero[8];    /* Padding */
};
```

## Process and Memory Management Compatibility

### Process Information Compatibility
Provides consistent process information across different process models:

```c
/* Process information compatibility */
struct compat_procinfo {
    int32_t     pid;            /* Process ID */
    int32_t     ppid;           /* Parent process ID */
    int32_t     pgid;           /* Process group ID */
    int32_t     sid;            /* Session ID */
    uint32_t    flags;          /* Process flags */
    compat_sigset_t sigmask;    /* Signal mask */
    int32_t     uid;            /* User ID */
    int32_t     gid;            /* Group ID */
};

/* Process operation compatibility */
int compat_getprocinfo(pid_t pid, struct compat_procinfo *info) {
    /* Translate process information to compatibility format */
    struct kinfo_proc native_info;
    int ret = getprocinfo(pid, &native_info);
    
    if (ret == 0) {
        info->pid = native_info.ki_pid;
        info->ppid = native_info.ki_ppid;
        /* ... additional field translations ... */
    }
    
    return ret;
}
```

### Memory Management Compatibility
Handles differences in memory management interfaces:

```c
/* Memory mapping compatibility */
struct compat_mmap_req {
    void        *addr;          /* Starting address */
    size_t      len;            /* Length */
    int         prot;           /* Protection */
    int         flags;          /* Flags */
    int         fd;             /* File descriptor */
    off_t       offset;         /* Offset */
};

/* Memory protection compatibility */
#define PROT_COMPAT_READ        0x1     /* Page can be read */
#define PROT_COMPAT_WRITE       0x2     /* Page can be written */
#define PROT_COMPAT_EXEC        0x4     /* Page can be executed */
#define PROT_COMPAT_NONE        0x0     /* Page can not be accessed */

/* Memory mapping translation */
void *compat_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) {
    /* Translate protection flags */
    int native_prot = translate_prot(prot);
    int native_flags = translate_flags(flags);
    
    return mmap(addr, len, native_prot, native_flags, fd, offset);
}
```

## Threading and Synchronization Compatibility

### Threading Interface Compatibility
Maintains compatibility across different threading implementations:

```c
/* Thread creation compatibility */
typedef uint32_t compat_pthread_t;
typedef uint32_t compat_pthread_attr_t;

struct compat_pthread_attr {
    uint32_t    detachstate;        /* Detach state */
    uint32_t    schedpolicy;        /* Scheduling policy */
    uint32_t    schedparam;         /* Scheduling parameter */
    uint32_t    inheritsched;       /* Inherit scheduling */
    uint32_t    scope;              /* Contention scope */
    size_t      guardsize;          /* Guard size */
    uint32_t    stackaddr;          /* Stack address */
    size_t      stacksize;          /* Stack size */
};

/* Thread operation translation */
int compat_pthread_create(compat_pthread_t *thread, 
                         const compat_pthread_attr_t *attr,
                         void *(*start_routine)(void*), void *arg) {
    pthread_t native_thread;
    pthread_attr_t native_attr;
    
    /* Translate thread attributes */
    translate_pthread_attr(attr, &native_attr);
    
    int ret = pthread_create(&native_thread, &native_attr, 
                            start_routine, arg);
    
    *thread = (compat_pthread_t)native_thread;
    return ret;
}
```

### Synchronization Primitive Compatibility
Provides consistent synchronization interfaces:

```c
/* Mutex compatibility */
typedef uint32_t compat_pthread_mutex_t;
typedef uint32_t compat_pthread_mutexattr_t;

struct compat_pthread_mutex_attr {
    uint32_t    pshared;            /* Process shared */
    uint32_t    type;               /* Mutex type */
    uint32_t    protocol;           /* Protocol */
    uint32_t    prioceiling;        /* Priority ceiling */
};

/* Condition variable compatibility */
typedef uint32_t compat_pthread_cond_t;
typedef uint32_t compat_pthread_condattr_t;

/* Semaphore compatibility */
typedef struct {
    uint32_t    sem_value;          /* Semaphore value */
    uint32_t    sem_flags;          /* Semaphore flags */
} compat_sem_t;
```

## Build System Integration

### Compatibility Layer Configuration
The compatibility layer integrates with the MINIX build system:

```makefile
# Compatibility layer makefile
COMPATDIR = ${.CURDIR}

# Compatibility header installation
INCLUDES += -I${COMPATDIR}/sys

# Compatibility library build
LIBCOMPAT = libcompat.a

# Architecture-specific compatibility
.if defined(MACHINE_ARCH)
. include "${COMPATDIR}/arch/${MACHINE_ARCH}.mk"
.endif
```

### Conditional Compatibility
Supports conditional compilation based on compatibility requirements:

```c
/* Conditional compatibility features */
#ifdef COMPAT_43
/* 4.3BSD compatibility */
#include "compat/bsd43.h"
#endif

#ifdef COMPAT_POSIX
/* POSIX compatibility */
#include "compat/posix.h"
#endif

#ifdef COMPAT_LINUX
/* Linux compatibility */
#include "compat/linux.h"
#endif
```

## Testing and Validation

### Compatibility Testing Framework
Comprehensive testing ensures compatibility layer correctness:

```c
/* Compatibility test framework */
struct compat_test_case {
    const char *name;               /* Test name */
    int     (*test_func)(void);     /* Test function */
    int     expected_result;        /* Expected result */
};

/* System call compatibility testing */
static struct compat_test_case syscall_compat_tests[] = {
    { "read_compat", test_read_compat, 0 },
    { "write_compat", test_write_compat, 0 },
    { "open_compat", test_open_compat, 0 },
    /* ... additional tests ... */
};
```

### Regression Testing
Ensures compatibility interfaces remain stable:

```c
/* Compatibility regression tests */
int compat_test_regression(void) {
    /* Test error code translation */
    for (int i = 0; i < nitems(minix_posix_errno_map); i++) {
        int translated = compat_translate_errno(
            minix_posix_errno_map[i].native_errno);
        assert(translated == minix_posix_errno_map[i].compat_errno);
    }
    
    /* Test data structure sizes */
    assert(sizeof(struct compat_stat) == COMPAT_STAT_SIZE);
    assert(sizeof(struct compat_timeval) == COMPAT_TIMEVAL_SIZE);
    /* ... additional regression tests ... */
}
```

The compatibility layer provides the essential foundation that enables MINIX 3 to maintain compatibility with existing software ecosystems while preserving the benefits of its microkernel architecture. This comprehensive compatibility framework ensures that applications developed for traditional UNIX systems can run seamlessly on MINIX without modification.