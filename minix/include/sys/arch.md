# MINIX System Interface Headers Architecture

## Directory Overview

The `/minix/include/sys/` directory contains MINIX-specific system interface definitions that extend standard POSIX system calls to support the microkernel architecture. These headers define the interface between user applications and MINIX system servers, providing specialized system calls, device I/O controls, and process management functions unique to the MINIX operating system.

## Microkernel Architecture Integration

### System Call Model
Unlike monolithic kernels, MINIX implements system services as userspace servers that communicate via message passing:
- **Process Manager (PM)**: Handles process creation, termination, and signals
- **Virtual File System (VFS)**: Manages file system operations and device access
- **Memory Manager (VM)**: Controls virtual memory allocation and paging
- **Reincarnation Server (RS)**: Monitors and restarts failed system services
- **Device Drivers**: Userspace drivers for hardware devices

### Message-Based IPC
System calls are implemented as synchronous message exchanges between user processes and system servers:
```c
// Traditional system call
int result = read(fd, buffer, count);

// MINIX message-based equivalent
message m;
m.m_type = VFS_READ;
m.VFS_FD = fd;
m.VFS_BUFFER = buffer;
m.VFS_COUNT = count;
_sendrec(VFS_PROC_NR, &m);
int result = m.VFS_RESULT;
```

## MINIX-Specific System Interfaces

### Process and Memory Management

#### `procfs.h` - Process File System Interface
**Purpose**: Provides debugging and monitoring interface for processes, primarily for GDB integration

**Core Structures**:
```c
// Process status information for debugging
#define PRSTATUS_VERSION    1       // Current version of prstatus_t

typedef struct prstatus {
    int     pr_version;             // Version number of struct
    size_t  pr_statussz;            // sizeof(prstatus_t)
    size_t  pr_gregsetsz;           // sizeof(gregset_t)
    size_t  pr_fpregsetsz;          // sizeof(fpregset_t)
    int     pr_osreldate;           // Kernel version
    int     pr_cursig;              // Current signal
    pid_t   pr_pid;                 // Process ID
    gregset_t pr_reg;               // General purpose registers
} prstatus_t;

// Process information for debugging
#define PRPSINFO_VERSION    1       // Current version of prpsinfo_t

typedef struct prpsinfo {
    int     pr_version;             // Version number of struct
    size_t  pr_psinfosz;            // sizeof(prpsinfo_t)
    char    pr_fname[MAXCOMLEN+1];  // Command name, null terminated
    char    pr_psargs[PRARGSZ+1];   // Arguments, null terminated
} prpsinfo_t;
```

**Architecture-Specific Register Access**:
```c
// RISC-V 64-bit register set integration
#ifdef __riscv
#include <riscv/reg.h>
// Map MINIX register structure to RISC-V registers
typedef struct riscv_regs gregset_t;
#endif
```

#### `statfs.h` - File System Statistics
**Purpose**: Provides file system statistics and mount information

**File System Statistics**:
```c
struct statfs {
    uint32_t f_type;                // File system type
    uint32_t f_bsize;               // Block size
    uint32_t f_blocks;              // Total blocks
    uint32_t f_bfree;               // Free blocks
    uint32_t f_bavail;              // Available blocks
    uint32_t f_files;               // Total inodes
    uint32_t f_ffree;               // Free inodes
    uint32_t f_fsid;                // File system ID
    uint32_t f_namelen;             // Maximum filename length
    uint32_t f_flags;               // Mount flags
    uint32_t f_spare[2];            // Reserved
};
```

### Device Input/Output Control

#### `ioc_net.h` - Network Device I/O Controls
**Purpose**: Defines I/O control commands for network devices and interfaces

**Network I/O Control Commands**:
```c
// Network I/O control base commands
#define NWIOSETHOPT     _IOW('n', 16, struct nwio_ethopt)    // Set Ethernet options
#define NWIOGETHOPT     _IOR('n', 17, struct nwio_ethopt)    // Get Ethernet options
#define NWIOSETPROTO    _IOW('n', 18, int)                  // Set protocol
#define NWIOGETPROTO    _IOR('n', 19, int)                  // Get protocol
#define NWIOSETRWFLAG   _IOW('n', 20, int)                  // Set read/write flags
#define NWIOGETRWFLAG   _IOR('n', 21, int)                  // Get read/write flags

// Message control for network operations
#define MSG_CONTROL_MAX (1024 - sizeof(socklen_t))

struct msg_control {
    char        msg_control[MSG_CONTROL_MAX];    // Control data
    socklen_t   msg_controllen;                 // Control data length
};
```

**Ethernet Options Structure**:
```c
struct nwio_ethopt {
    uint32_t    nweo_flags;         // Option flags
    uint8_t     nweo_multiaddr[6];  // Multicast address
    uint8_t     nweo_addr[6];       // Ethernet address
    uint8_t     nweo_type;          // Ethernet type
};

// Ethernet option flags
#define NWEO_EN_LOC     0x01        // Enable local access
#define NWEO_EN_BROAD   0x02        // Enable broadcast
#define NWEO_EN_MULTI   0x04        // Enable multicast
#define NWEO_EN_PROMISC 0x08        // Enable promiscuous mode
#define NWEO_EN_COPY    0x10        // Enable copy mode
#define NWEO_DI_LOC     0x20        // Disable local access
#define NWEO_DI_BROAD   0x40        // Disable broadcast
#define NWEO_DI_MULTI   0x80        // Disable multicast
```

#### `ioc_block.h` - Block Device I/O Controls
**Purpose**: Defines I/O control commands for block devices (disks)

**Block Device Commands**:
```c
// Block device control commands
#define B_IOCTL_GETSIZE _IOR('B', 1, uint64_t)   // Get device size
#define B_IOCTL_SETLABEL _IOW('B', 2, void *)   // Set disk label
#define B_IOCTL_GETLABEL _IOR('B', 3, void *)   // Get disk label
#define B_IOCTL_FLUSH    _IO('B', 4)            // Flush cache
#define B_IOCTL_BYPASS   _IOW('B', 5, int)      // Set bypass mode
```

#### `ioc_disk.h` - Disk Device I/O Controls
**Purpose**: Defines I/O control commands for disk devices

**Disk Device Commands**:
```c
// Disk geometry information
struct disk_geometry {
    uint32_t cylinders;             // Number of cylinders
    uint32_t heads;                 // Number of heads
    uint32_t sectors;               // Sectors per track
    uint32_t sector_size;           // Sector size in bytes
};

// Disk I/O control commands
#define D_IOCTL_GET_GEOMETRY _IOR('D', 1, struct disk_geometry)
#define D_IOCTL_SET_GEOMETRY _IOW('D', 2, struct disk_geometry)
#define D_IOCTL_GET_PARTITION _IOR('D', 3, struct partition_entry)
#define D_IOCTL_SET_PARTITION _IOW('D', 4, struct partition_entry)
```

#### `ioc_file.h` - File System I/O Controls
**Purpose**: Defines I/O control commands for file system operations

**File System Commands**:
```c
// File system control commands
#define F_IOCTL_GETFLAGS _IOR('F', 1, uint32_t)   // Get file flags
#define F_IOCTL_SETFLAGS _IOW('F', 2, uint32_t)   // Set file flags
#define F_IOCTL_GETLABEL _IOR('F', 3, char *)     // Get file label
#define F_IOCTL_SETLABEL _IOW('F', 4, char *)     // Set file label
```

#### `ioc_memory.h` - Memory Device I/O Controls
**Purpose**: Defines I/O control commands for memory devices

**Memory Device Commands**:
```c
// Memory device control commands
#define M_IOCTL_GETSIZE  _IOR('M', 1, size_t)     // Get memory size
#define M_IOCTL_GETFLAGS _IOR('M', 2, uint32_t)   // Get memory flags
#define M_IOCTL_SETFLAGS _IOW('M', 3, uint32_t)   // Set memory flags
#define M_IOCTL_LOCK     _IOW('M', 4, void *)     // Lock memory region
#define M_IOCTL_UNLOCK   _IOW('M', 5, void *)     // Unlock memory region
```

#### `ioc_sound.h` - Audio Device I/O Controls
**Purpose**: Defines I/O control commands for audio devices

**Audio Device Commands**:
```c
// Audio format information
struct audio_format {
    uint32_t sample_rate;           // Samples per second
    uint32_t channels;              // Number of channels
    uint32_t bits_per_sample;       // Bits per sample
    uint32_t encoding;              // Encoding format
};

// Audio control commands
#define S_IOCTL_GET_FORMAT _IOR('S', 1, struct audio_format)
#define S_IOCTL_SET_FORMAT _IOW('S', 2, struct audio_format)
#define S_IOCTL_GET_VOLUME _IOR('S', 3, uint32_t)
#define S_IOCTL_SET_VOLUME _IOW('S', 4, uint32_t)
#define S_IOCTL_START      _IO('S', 5)                 // Start audio
#define S_IOCTL_STOP       _IO('S', 6)                 // Stop audio
#define S_IOCTL_PAUSE      _IO('S', 7)                 // Pause audio
#define S_IOCTL_RESUME     _IO('S', 8)                 // Resume audio
```

#### `ioc_fb.h` - Framebuffer Device I/O Controls
**Purpose**: Defines I/O control commands for framebuffer devices

**Framebuffer Commands**:
```c
// Framebuffer information
struct fb_info {
    uint32_t width;                 // Width in pixels
    uint32_t height;                // Height in pixels
    uint32_t depth;                 // Color depth in bits
    uint32_t pitch;                 // Bytes per line
    uint32_t pixel_format;          // Pixel format
    void    *fb_addr;               // Framebuffer address
    size_t   fb_size;               // Framebuffer size
};

// Framebuffer control commands
#define FB_IOCTL_GET_INFO   _IOR('f', 1, struct fb_info)
#define FB_IOCTL_SET_MODE   _IOW('f', 2, struct fb_info)
#define FB_IOCTL_GET_PALETTE _IOR('f', 3, void *)
#define FB_IOCTL_SET_PALETTE _IOW('f', 4, void *)
#define FB_IOCTL_BLANK      _IOW('f', 5, int)          // Blank screen
#define FB_IOCTL_UNBLANK    _IO('f', 6)                 // Unblank screen
```

#### `ioc_fbd.h` - Framebuffer Display I/O Controls
**Purpose**: Defines I/O control commands for framebuffer display devices

**Display Control Commands**:
```c
// Display control commands
#define FBD_IOCTL_GET_MODE  _IOR('F', 1, struct display_mode)
#define FBD_IOCTL_SET_MODE  _IOW('F', 2, struct display_mode)
#define FBD_IOCTL_GET_POWER _IOR('F', 3, int)
#define FBD_IOCTL_SET_POWER _IOW('F', 4, int)
#define FBD_IOCTL_GET_BACKLIGHT _IOR('F', 5, uint32_t)
#define FBD_IOCTL_SET_BACKLIGHT _IOW('F', 6, uint32_t)
```

#### `ioc_tape.h` - Tape Device I/O Controls
**Purpose**: Defines I/O control commands for tape devices

**Tape Device Commands**:
```c
// Tape control commands
#define T_IOCTL_REWIND      _IO('T', 1)                 // Rewind tape
#define T_IOCTL_ERASE       _IO('T', 2)                 // Erase tape
#define T_IOCTL_WEOF        _IO('T', 3)                 // Write EOF
#define T_IOCTL_SEEK        _IOW('T', 4, uint32_t)      // Seek to block
#define T_IOCTL_TELL        _IOR('T', 5, uint32_t)      // Get position
```

### System Control Interface

#### `svrctl.h` - System Control Interface
**Purpose**: Provides system control and configuration interface for MINIX system parameters

**System Control Commands**:
```c
// System control request structure
struct svrctl_req {
    int     request;                // Control request code
    void   *data;                   // Request data
    size_t  size;                   // Data size
};

// System control request codes
#define SVRCTL_GET_PARAM    1       // Get system parameter
#define SVRCTL_SET_PARAM    2       // Set system parameter
#define SVRCTL_GET_STATS    3       // Get system statistics
#define SVRCTL_RESET_STATS  4       // Reset system statistics
#define SVRCTL_SHUTDOWN     5       // System shutdown
#define SVRCTL_REBOOT       6       // System reboot
```

**System Parameters**:
```c
// System parameter categories
#define SVRCTL_PARAM_KERNEL 0x1000    // Kernel parameters
#define SVRCTL_PARAM_PM     0x2000    // Process manager parameters
#define SVRCTL_PARAM_VFS    0x3000    // VFS parameters
#define SVRCTL_PARAM_MEMORY 0x4000    // Memory parameters
#define SVRCTL_PARAM_NET    0x5000    // Network parameters
```

### Video and Display Interface

#### `video.h` - Video Interface Definitions
**Purpose**: Defines interfaces for video devices and display management

**Video Mode Structure**:
```c
// Video mode information
struct video_mode {
    uint32_t width;                 // Width in pixels
    uint32_t height;                // Height in pixels
    uint32_t refresh_rate;          // Refresh rate in Hz
    uint32_t flags;                 // Mode flags
};

// Video control commands
#define VIDEO_IOCTL_GET_MODE    _IOR('v', 1, struct video_mode)
#define VIDEO_IOCTL_SET_MODE    _IOW('v', 2, struct video_mode)
#define VIDEO_IOCTL_GET_CAPABILITIES _IOR('v', 3, uint32_t)
#define VIDEO_IOCTL_QUERY_MODES _IOR('v', 4, struct video_mode *)
```

#### `ioc_fb.h` - Framebuffer Interface (Duplicate for consistency)
**Purpose**: Framebuffer interface definitions for display devices

### Keyboard Interface

#### `kbdio.h` - Keyboard I/O Interface
**Purpose**: Defines keyboard input interface and control commands

**Keyboard Commands**:
```c
// Keyboard control commands
#define K_IOCTL_GET_MODE    _IOR('k', 1, int)       // Get keyboard mode
#define K_IOCTL_SET_MODE    _IOW('k', 2, int)       // Set keyboard mode
#define K_IOCTL_GET_LEDS    _IOR('k', 3, uint8_t)   // Get LED status
#define K_IOCTL_SET_LEDS    _IOW('k', 4, uint8_t)   // Set LED status
#define K_IOCTL_BELL        _IOW('k', 5, int)       // Generate bell
#define K_IOCTL_REBOOT      _IO('k', 6)              // Reboot key sequence
```

**Keyboard Modes**:
```c
#define K_MODE_RAW      0               // Raw scancode mode
#define K_MODE_XLATE    1               // Translated mode
#define K_MODE_MEDIUM   2               // Medium raw mode
#define K_MODE_UNICODE  3               // Unicode mode
```

## RISC-V 64-bit Architecture Support

### Memory Alignment Requirements
```c
// Aligned structures for RISC-V 64-bit
struct svrctl_req {
    int     request;                // 4 bytes (aligned)
    void   *data;                   // 8 bytes on RV64 (aligned)
    size_t  size;                   // 8 bytes on RV64 (aligned)
} __attribute__((aligned(8)));

struct fb_info {
    uint32_t width;                 // 4 bytes
    uint32_t height;                // 4 bytes
    uint32_t depth;                 // 4 bytes
    uint32_t pitch;                 // 4 bytes
    uint32_t pixel_format;          // 4 bytes
    void    *fb_addr;               // 8 bytes on RV64 (aligned)
    size_t   fb_size;               // 8 bytes on RV64 (aligned)
} __attribute__((aligned(8)));
```

### Register Integration
```c
// RISC-V register integration for procfs
#ifdef __riscv
struct riscv_debug_regs {
    uint64_t zero;                  // x0 - zero
    uint64_t ra;                    // x1 - return address
    uint64_t sp;                    // x2 - stack pointer
    uint64_t gp;                    // x3 - global pointer
    uint64_t tp;                    // x4 - thread pointer
    uint64_t t0;                    // x5 - temporary 0
    uint64_t t1;                    // x6 - temporary 1
    uint64_t t2;                    // x7 - temporary 2
    uint64_t s0;                    // x8 - saved register 0
    uint64_t s1;                    // x9 - saved register 1
    uint64_t a0;                    // x10 - argument 0
    uint64_t a1;                    // x11 - argument 1
    uint64_t a2;                    // x12 - argument 2
    uint64_t a3;                    // x13 - argument 3
    uint64_t a4;                    // x14 - argument 4
    uint64_t a5;                    // x15 - argument 5
    uint64_t a6;                    // x16 - argument 6
    uint64_t a7;                    // x17 - argument 7
    uint64_t s2;                    // x18 - saved register 2
    uint64_t s3;                    // x19 - saved register 3
    uint64_t s4;                    // x20 - saved register 4
    uint64_t s5;                    // x21 - saved register 5
    uint64_t s6;                    // x22 - saved register 6
    uint64_t s7;                    // x23 - saved register 7
    uint64_t s8;                    // x24 - saved register 8
    uint64_t s9;                    // x25 - saved register 9
    uint64_t s10;                   // x26 - saved register 10
    uint64_t s11;                   // x27 - saved register 11
    uint64_t t3;                    // x28 - temporary 3
    uint64_t t4;                    // x29 - temporary 4
    uint64_t t5;                    // x30 - temporary 5
    uint64_t t6;                    // x31 - temporary 6
    uint64_t pc;                    // Program counter
};
#endif
```

### Performance Optimizations
- **64-bit Arithmetic**: Efficient 64-bit calculations for device parameters
- **Memory Access**: Aligned I/O control structure access
- **System Call Interface**: Optimized message passing for RV64
- **Cache Efficiency**: Structure layout optimized for cache lines

## MINIX System Integration

### System Call Message Interface
```c
// Generic system control interface
int do_svrctl(struct svrctl_req *req) {
    int result;
    message m;
    
    // Prepare system control message
    m.m_type = SVRCTL_REQUEST;
    m.SVRCTL_REQUEST = req->request;
    m.SVRCTL_DATA = req->data;
    m.SVRCTL_SIZE = req->size;
    
    // Send to system server
    result = _sendrec(SYSTEM_PROC_NR, &m);
    if (result != OK)
        return result;
    
    // Extract result
    return m.SVRCTL_RESULT;
}
```

### Device Driver Integration
```c
// Generic device I/O control interface
int device_ioctl(int device, unsigned long request, void *data) {
    message m;
    int result;
    
    // Prepare device control message
    m.m_type = DEV_IOCTL;
    m.DEVICE = device;
    m.REQUEST = request;
    m.DATA_PTR = data;
    
    // Send to device driver
    result = _sendrec(DEVICE_PROC_NR, &m);
    if (result != OK)
        return result;
    
    // Extract result
    return m.IOCTL_RESULT;
}
```

### Multi-Server Coordination
```c
// Coordinated system shutdown
void system_shutdown(int how) {
    message m;
    
    // Notify all servers of shutdown
    m.m_type = SVRCTL_SHUTDOWN;
    m.SHUTDOWN_HOW = how;
    
    // Notify VFS server
    _sendrec(VFS_PROC_NR, &m);
    
    // Notify PM server
    _sendrec(PM_PROC_NR, &m);
    
    // Notify VM server
    _sendrec(VM_PROC_NR, &m);
    
    // Final system shutdown
    _sendrec(SYSTEM_PROC_NR, &m);
}
```

## Security Framework

### System Control Security
```c
// System control access control
struct svrctl_acl {
    uid_t   uid;                    // User ID
    uint32_t permissions;           // Permission bits
};

// Permission bits
#define SVRCTL_PERM_READ    0x01    // Read system parameters
#define SVRCTL_PERM_WRITE   0x02    // Write system parameters
#define SVRCTL_PERM_ADMIN   0x04    // Administrative operations
#define SVRCTL_PERM_SHUTDOWN 0x08   // Shutdown/reboot operations
```

### Device Access Control
```c
// Device access control structure
struct device_acl {
    char    device_name[32];        // Device name
    uid_t   owner;                  // Device owner
    gid_t   group;                  // Device group
    mode_t  permissions;            // Access permissions
    uint32_t flags;                 // Special flags
};

// Special device flags
#define DEVICE_FLAG_EXCLUSIVE   0x01  // Exclusive access
#define DEVICE_FLAG_APPEND      0x02  // Append-only access
#define DEVICE_FLAG_IMMUTABLE   0x04  // Immutable device
```

### Audit and Logging
```c
// System audit structure
struct system_audit {
    uint32_t event_type;            // Event type
    uid_t    user_id;               // User ID
    pid_t    process_id;            // Process ID
    time_t   timestamp;             // Event timestamp
    char     details[256];          // Event details
};

// Audit event types
#define AUDIT_SVRCTL_REQUEST    1   // System control request
#define AUDIT_DEVICE_ACCESS     2   // Device access
#define AUDIT_SYSTEM_SHUTDOWN   3   // System shutdown
#define AUDIT_SECURITY_VIOLATION 4  // Security violation
```

## Development Guidelines

### System Control Development
```c
// Custom system control implementation
int my_svrctl_handler(int request, void *data, size_t size) {
    switch (request) {
    case MY_SVRCTL_GET_INFO:
        if (size < sizeof(struct my_info))
            return EINVAL;
        return my_get_info(data);
        
    case MY_SVRCTL_SET_CONFIG:
        if (!is_admin_user())
            return EPERM;
        return my_set_config(data, size);
        
    default:
        return EINVAL;
    }
}
```

### Device Driver Development
```c
// Device driver I/O control implementation
int my_device_ioctl(devminor_t minor, unsigned long request, 
                   endpoint_t endpt, cp_grant_id_t grant, int flags) {
    struct my_device_info info;
    int result;
    
    switch (request) {
    case MY_IOCTL_GET_INFO:
        // Get device information
        my_get_device_info(minor, &info);
        result = sys_safecopyto(endpt, grant, 0, (vir_bytes)&info, 
                               sizeof(info));
        return result;
        
    case MY_IOCTL_SET_MODE:
        // Set device mode
        result = sys_safecopyfrom(endpt, grant, 0, (vir_bytes)&info, 
                                 sizeof(info));
        if (result != OK)
            return result;
        return my_set_device_mode(minor, &info);
        
    default:
        return EINVAL;
    }
}
```

### Error Handling Pattern
```c
// Comprehensive system error handling
const char *svrctl_err_string(int error) {
    switch (error) {
    case EPERM:
        return "Operation not permitted";
    case ENOENT:
        return "No such file or directory";
    case ESRCH:
        return "No such process";
    case EINTR:
        return "Interrupted system call";
    case EIO:
        return "I/O error";
    case ENXIO:
        return "No such device or address";
    case E2BIG:
        return "Argument list too long";
    case ENOEXEC:
        return "Exec format error";
    case EBADF:
        return "Bad file number";
    case EAGAIN:
        return "Try again";
    case ENOMEM:
        return "Out of memory";
    case EACCES:
        return "Permission denied";
    case EFAULT:
        return "Bad address";
    case EBUSY:
        return "Device or resource busy";
    case EEXIST:
        return "File exists";
    case ENODEV:
        return "No such device";
    case ENOTDIR:
        return "Not a directory";
    case EISDIR:
        return "Is a directory";
    case EINVAL:
        return "Invalid argument";
    default:
        return "Unknown error";
    }
}
```

## Testing and Validation

### System Interface Testing
- **Message Passing**: Validate IPC message integrity
- **System Call Coverage**: Test all system control interfaces
- **Error Handling**: Verify proper error reporting
- **Performance**: Measure system call latency

### Device I/O Testing
- **Command Validation**: Test all I/O control commands
- **Parameter Validation**: Verify input parameter checking
- **Device Compatibility**: Test across different device types
- **Concurrency**: Test simultaneous device access

### Security Testing
- **Access Control**: Validate permission checking
- **Input Validation**: Test malformed input handling
- **Privilege Escalation**: Prevent unauthorized operations
- **Audit Trail**: Verify security event logging

This comprehensive MINIX system interface collection provides the foundation for microkernel-based system management, maintaining full compatibility with standard interfaces while extending functionality for the message-passing architecture and optimizing for the RISC-V 64-bit platform.