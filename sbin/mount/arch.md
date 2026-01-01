# MINIX Filesystem Mount Utility (mount)

## Overview

The `mount` utility is a critical system administration tool for attaching filesystems to the MINIX directory tree. It provides comprehensive filesystem mounting capabilities, including device detection, mount option management, and filesystem type auto-discovery. This utility is essential for making filesystems accessible to the system and users, managing storage devices, and maintaining the hierarchical filesystem structure that is fundamental to UNIX-like operating systems.

## Architecture

### Core Functionality
- **Filesystem Attachment**: Mount devices to mount points
- **Auto-Detection**: Automatic filesystem type recognition
- **Mount Options**: Comprehensive option handling
- **Mount Table Management**: /etc/mtab maintenance
- **Multiple Filesystem Support**: Various filesystem types
- **Device Management**: Block device interface
- **Error Handling**: Comprehensive error reporting

### MINIX Integration
- Communicates with VFS server for mount operations
- Uses MINIX message-passing for system calls
- Integrates with PM server for privilege management
- Supports MINIX-specific filesystem features
- Service-oriented architecture compliance

## Source Files

### Main Implementation
- **`mount.c`** (MINIX-specific implementation)
  - Main program entry point and command dispatcher
  - Command-line argument parsing and validation
  - Filesystem type auto-detection logic
  - Mount system call interface
  - Mount table (/etc/mtab) management
  - Error handling and user feedback
  - Integration with MINIX system services

### Utility Files in /sbin/mount/
- **`fattr.c`** (2,421 lines)
  - File attribute conversion utilities
  - Permission and ownership handling
  - Attribute parsing and validation
  - Mount option processing helpers
  - Filesystem-specific attribute mapping

- **`mountprog.h`** (1,717 lines)
  - Common mount program interface
  - Utility function declarations
  - Attribute conversion prototypes
  - Path manipulation functions
  - Filesystem name validation

### Filesystem Type Detection
```c
// Auto-detection logic from mount.c
if ((type == NULL || !strcmp(type, MINIX_FS_TYPE)) && device != NULL) {
    v = fsversion(device, "mount");
    switch (v) {
        case FSVERSION_MFS1:
        case FSVERSION_MFS2:
        case FSVERSION_MFS3: type = MINIX_FS_TYPE; break;
        case FSVERSION_EXT2: type = "ext2"; break;
        case FSVERSION_ISO9660: type = "isofs"; break;
    }
}
```

## Build Configuration

### Makefile Structure
The mount utility is built as part of the MINIX commands collection:
- Located in `/minix/commands/mount/`
- Integrated with MINIX build system
- Cross-compilation support for RISC-V
- Library dependencies for filesystem operations

### Dependencies
- **System Libraries**: libc, libutil, libminix
- **Filesystem Libraries**: libfs, libminixfs
- **MINIX Libraries**: libsys, libtimers
- **Header Files**: sys/mount.h, fstab.h, minix/minlib.h

### Build Requirements
- C compiler with C99 support
- MINIX cross-compilation toolchain
- Filesystem type definitions
- Mount table support libraries

## MINIX System Integration

### Server Communication
```c
// Mount system call interface
int minix_mount(const char *device, const char *mountpoint, 
                int mountflags, int srvflags, 
                const char *type, const char *args);

// Message-based communication with VFS
message m;
m.m_type = VFS_MOUNT;
m.VFS_MOUNT_DEVICE = device;
m.VFS_MOUNT_POINT = mountpoint;
m.VFS_MOUNT_FLAGS = mountflags;
m.VFS_MOUNT_TYPE = type;
_sendrec(VFS_PROC_NR, &m);
```

### Service Dependencies
- **VFS Server**: Primary filesystem interface
- **PM Server**: Process management and privileges
- **DS Server**: Service discovery for filesystem servers
- **MEM Server**: Memory management for mount operations

### Filesystem Server Architecture
MINIX uses a service-oriented approach where each filesystem type has its own server:
- **MFS Server**: MINIX filesystem server
- **EXT2 Server**: Extended filesystem server
- **ISOFS Server**: ISO9660 CD-ROM filesystem server
- **MFS/EXT2/ISOFS**: Individual filesystem processes

## Mount Operation Process

### Mount Sequence
1. **Argument Parsing**: Process command-line options
2. **Privilege Checking**: Verify root privileges
3. **Device Validation**: Check device accessibility
4. **Filesystem Detection**: Auto-detect filesystem type
5. **Mount Point Verification**: Validate mount point directory
6. **Server Startup**: Launch appropriate filesystem server
7. **System Call**: Perform mount operation
8. **Mount Table Update**: Update /etc/mtab

### Filesystem Type Detection
```c
// Filesystem version detection
int fsversion(const char *device, const char *progname);
```

### Mount Options Processing
- **Read-Only**: Mount filesystem read-only
- **No-Suid**: Disable set-user-ID execution
- **Nodev**: Don't interpret device files
- **Noexec**: Don't allow execution
- **Reuse**: Reuse existing filesystem server
- **Existing**: Use existing mount instance

## Filesystem Type Support

### Native MINIX Filesystems
- **MFS**: MINIX filesystem (default)
- **MFS1**: MINIX filesystem version 1
- **MFS2**: MINIX filesystem version 2  
- **MFS3**: MINIX filesystem version 3

### Foreign Filesystem Support
- **EXT2**: Extended filesystem (read/write)
- **ISO9660**: ISO CD-ROM filesystem (read-only)
- **MSDOS**: MS-DOS FAT filesystem (via separate utility)

### Filesystem Features
| Filesystem | Read | Write | Auto-Detect | Server Process |
|------------|------|-------|-------------|----------------|
| MFS | ✓ | ✓ | ✓ | mfs |
| EXT2 | ✓ | ✓ | ✓ | ext2 |
| ISO9660 | ✓ | ✗ | ✓ | isofs |

## Mount Table Management

### /etc/mtab Structure
The mount table maintains information about currently mounted filesystems:
```c
// Mount table entry structure
struct mtab_entry {
    char *device;      // Device name
    char *mountpoint;  // Mount point path
    char *type;        // Filesystem type
    char *options;     // Mount options
    int freq;          // Dump frequency
    int passno;        // fsck pass number
};
```

### Mount Table Operations
- **Load**: Read current mount table
- **Update**: Add new mount entries
- **Remove**: Delete unmount entries
- **List**: Display mounted filesystems
- **Verify**: Validate mount table consistency

### Mount All Functionality
```c
// Mount all filesystems from /etc/fstab
int mount_all(void);
```

## RISC-V 64-bit Platform Considerations

### Architecture Independence
- Portable C implementation
- No assembly dependencies
- Platform-agnostic device handling
- Cross-architecture compatibility

### RISC-V Specific Features
- Memory-aligned data structures
- Efficient system call interface
- Optimized string operations
- Atomic operations for mount counters

### Device Interface
- Standard MINIX block device interface
- Platform-independent device access
- VirtIO block device support
- Memory-mapped I/O compatibility

### Performance Optimizations
- Efficient mount table operations
- Fast filesystem detection
- Minimal system call overhead
- Optimized memory allocation

## Error Handling and Diagnostics

### Error Categories
- **Device Errors**: Block device access failures
- **Filesystem Errors**: Corrupted or unsupported filesystems
- **Permission Errors**: Insufficient privileges
- **Mount Point Errors**: Invalid mount directories
- **Server Errors**: Filesystem server failures
- **System Call Errors**: Kernel mount failures

### Error Recovery
```c
// Error handling example
if (minix_mount(device, mountpoint, flags, srvflags, type, args) < 0) {
    err = strerror(errno);
    fprintf(stderr, "mount: Can't mount %s on %s: %s\n",
            device, mountpoint, err);
    return(EXIT_FAILURE);
}
```

### Diagnostic Features
- Comprehensive error messages
- Filesystem type detection reporting
- Mount table status display
- Debug mode for troubleshooting
- Verbose output option

## Security Considerations

### Access Control
- Root privilege requirement for mount operations
- Device access permission checking
- Mount point directory permissions
- Filesystem capability validation

### Secure Mount Options
- **Nodev**: Prevent device file interpretation
- **Noexec**: Disable executable files
- **Nosuid**: Prevent set-user-ID execution
- **Read-only**: Prevent write operations

### Input Validation
- Device path validation
- Mount point path verification
- Filesystem type checking
- Option parameter validation
- Buffer overflow protection

## Usage Examples

### Basic Mounting
```bash
# Mount MINIX filesystem
mount /dev/hd1 /mnt

# Mount with specific type
mount -t ext2 /dev/hd2 /mnt2

# Mount read-only
mount -r /dev/cd0 /cdrom

# Display mounted filesystems
mount
```

### Advanced Usage
```bash
# Mount all filesystems from /etc/fstab
mount -a

# Mount with options
mount -o nodev,noexec /dev/hd1 /secure

# Auto-detect and mount
mount /dev/fd0 /floppy
```

### System Integration
```bash
# Boot-time mounting (from /etc/rc)
mount -a

# Remount with different options
mount -u -o remount,ro /

# Mount without updating /etc/mtab
mount -n /dev/hd1 /tmp
```

## Testing and Validation

### Functional Testing
- Filesystem mounting/unmounting
- Auto-detection accuracy
- Mount table management
- Error condition handling

### Filesystem Compatibility
- Cross-filesystem mounting
- Option compatibility
- Server communication
- Device interface testing

### Platform Testing
- RISC-V 64-bit validation
- Cross-compilation testing
- Device driver integration
- System call interface testing

## Maintenance and Updates

### NetBSD Synchronization
- Periodic updates from NetBSD
- Security patches and fixes
- Filesystem enhancements
- Compatibility improvements

### MINIX-Specific Enhancements
- Service-oriented architecture
- Message-based system integration
- Enhanced reliability features
- Microkernel optimization

### Filesystem Evolution
- New filesystem type support
- Enhanced auto-detection
- Improved error handling
- Performance optimizations

The `mount` utility provides essential filesystem mounting capabilities that are fundamental to MINIX system operation. Its integration with MINIX's service-oriented architecture, comprehensive filesystem support, and robust error handling make it a critical component for storage management in the microkernel environment.