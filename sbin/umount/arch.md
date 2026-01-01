# MINIX Filesystem Unmount Utility (umount)

## Overview

The `umount` utility is a critical system administration tool for detaching filesystems from the MINIX directory tree. It provides comprehensive filesystem unmounting capabilities, including graceful shutdown of filesystem services, cleanup of mount points, and management of active filesystem references. This utility is essential for safely removing storage devices, performing filesystem maintenance, and managing system storage resources in MINIX systems.

## Architecture

### Core Functionality
- **Filesystem Detachment**: Safely unmount mounted filesystems
- **Service Shutdown**: Graceful filesystem server termination
- **Reference Management**: Handle active filesystem references
- **Force Unmount**: Emergency unmount capabilities
- **Mount Table Updates**: Maintain /etc/mtab consistency
- **Device Cleanup**: Release device resources
- **Error Recovery**: Handle unmount failures

### MINIX Integration
- Communicates with VFS server for unmount operations
- Uses MINIX message-passing for system calls
- Integrates with filesystem servers for cleanup
- Supports MINIX service-oriented architecture
- Coordinates with PM server for process management

## Source Files

### Main Implementation
- **`umount.c`** (Located in `/minix/commands/umount/`)
  - Main program entry point and command dispatcher
  - Command-line argument parsing and validation
  - Unmount system call interface
  - Mount table (/etc/mtab) maintenance
  - Error handling and user feedback
  - Integration with MINIX system services
  - Force unmount logic for emergency situations

### Utility Functions
- **Mount table management functions**
  - /etc/mtab parsing and updating
  - Mounted filesystem enumeration
  - Device-to-mountpoint mapping
  - Filesystem reference counting

- **Error handling utilities**
  - Comprehensive error reporting
  - System error interpretation
  - User-friendly error messages
  - Recovery suggestions

## Build Configuration

### Integration with MINIX Commands
The umount utility is built as part of the MINIX commands collection:
- Located in `/minix/commands/umount/`
- Integrated with MINIX build system
- Cross-compilation support for RISC-V
- Library dependencies for filesystem operations

### Dependencies
- **System Libraries**: libc, libutil, libminix
- **Filesystem Libraries**: libfs, libminixfs
- **MINIX Libraries**: libsys, libtimers
- **Header Files**: sys/mount.h, fstab.h, unistd.h

### Build Requirements
- C compiler with C99 support
- MINIX cross-compilation toolchain
- Filesystem type definitions
- Mount table support libraries

## MINIX System Integration

### Server Communication
```c
// Unmount system call interface
int umount(const char *mountpoint);
int unmount(const char *mountpoint, int flags);

// Message-based communication with VFS
message m;
m.m_type = VFS_UNMOUNT;
m.VFS_UNMOUNT_POINT = mountpoint;
m.VFS_UNMOUNT_FLAGS = flags;
_sendrec(VFS_PROC_NR, &m);
```

### Service Dependencies
- **VFS Server**: Primary filesystem interface for unmount
- **Filesystem Servers**: Individual filesystem cleanup (mfs, ext2, isofs)
- **PM Server**: Process management and privilege verification
- **DS Server**: Service discovery and server termination

### Filesystem Server Shutdown
MINIX uses a service-oriented approach where unmounting involves:
- **Reference Counting**: Ensure no active references
- **Cache Flushing**: Write cached data to disk
- **Server Termination**: Graceful filesystem server shutdown
- **Resource Cleanup**: Release allocated resources
- **Device Release**: Return device to available pool

## Unmount Operation Process

### Standard Unmount Sequence
1. **Argument Parsing**: Process command-line options
2. **Privilege Checking**: Verify root privileges
3. **Mount Point Validation**: Verify mount point exists
4. **Reference Check**: Ensure no active references
5. **Filesystem Sync**: Flush pending I/O operations
6. **Server Coordination**: Notify filesystem server
7. **Unmount Operation**: Perform system unmount
8. **Mount Table Update**: Remove from /etc/mtab

### Force Unmount Sequence
1. **Override Checks**: Bypass active reference checks
2. **Process Termination**: Kill processes using filesystem
3. **Emergency Sync**: Force write pending data
4. **Immediate Unmount**: Perform unmount without delay
5. **Cleanup**: Remove mount table entry

### Unmount All Functionality
```c
// Unmount all filesystems (except root)
int umount_all(int flags);
```

## Filesystem Type Handling

### Supported Filesystems
- **MFS**: MINIX filesystem
- **EXT2**: Extended filesystem  
- **ISO9660**: ISO CD-ROM filesystem
- **MSDOS**: MS-DOS FAT filesystem

### Filesystem-Specific Cleanup
Each filesystem server handles its own cleanup:
- **Cache Flushing**: Write dirty buffers
- **Metadata Sync**: Update filesystem metadata
- **Lock Release**: Release filesystem locks
- **Resource Deallocation**: Free allocated memory
- **Device Close**: Close block device

### Mount Point Management
```c
// Mount table operations
int load_mtab(const char *program);
int update_mtab_remove(const char *mountpoint);
int find_mtab_entry(const char *device, const char *mountpoint);
```

## Command-Line Interface

### Basic Usage
```bash
# Unmount by mount point
umount /mnt

# Unmount by device
umount /dev/hd1

# Force unmount
umount -f /mnt

# Unmount all (except root)
umount -a
```

### Advanced Options
- **Force (-f)**: Force unmount despite active references
- **All (-a)**: Unmount all mounted filesystems
- **Verbose (-v)**: Display detailed progress
- **Dry-run (-n)**: Don't update /etc/mtab
- **Recursive (-R)**: Unmount recursively

## RISC-V 64-bit Platform Considerations

### Architecture Independence
- Portable C implementation
- No assembly dependencies
- Platform-agnostic device handling
- Cross-architecture compatibility

### RISC-V Specific Features
- Memory-aligned data structures
- Efficient system call interface
- Atomic operations for reference counting
- Optimized string operations

### Device Interface
- Standard MINIX block device interface
- Platform-independent device access
- VirtIO block device support
- Memory-mapped I/O compatibility

### Performance Optimizations
- Efficient mount table operations
- Fast reference counting
- Minimal system call overhead
- Optimized memory allocation

## Error Handling and Diagnostics

### Common Error Conditions
- **Device Busy**: Filesystem has active references
- **Invalid Mount Point**: Mount point doesn't exist
- **Permission Denied**: Insufficient privileges
- **Not Mounted**: Specified path not a mount point
- **I/O Error**: Device access failures
- **Server Error**: Filesystem server problems

### Recovery Mechanisms
- **Force Option**: Override busy checks
- **Process Identification**: Show blocking processes
- **Retry Logic**: Attempt unmount multiple times
- **Cleanup**: Remove stale mount entries

## Security Considerations

### Access Control
- Root privilege requirement for unmount operations
- Mount point access permission checking
- Device access permission verification
- Filesystem capability validation

### Input Validation
- Mount point path validation
- Device path verification
- Option parameter checking
- Buffer overflow protection

## Usage Examples

### Basic Unmounting
```bash
# Unmount a filesystem
umount /mnt/data

# Unmount by device name
umount /dev/hd1

# Force unmount (emergency)
umount -f /mnt/stuck

# Unmount all filesystems
umount -a
```

### System Integration
```bash
# Shutdown sequence
umount -a  # Unmount all filesystems

# Safe device removal
sync && umount /mnt/usb && eject /dev/usb0
```

## Testing and Validation

### Functional Testing
- Successful unmount operations
- Error condition handling
- Force unmount functionality
- Mount table consistency

### Platform Testing
- RISC-V 64-bit validation
- Cross-compilation testing
- Device driver integration
- System call interface testing

The `umount` utility provides essential filesystem detachment capabilities that are fundamental to MINIX system maintenance and storage management. Its robust error handling, comprehensive option support, and tight integration with MINIX system services make it a critical component for safe filesystem management in the microkernel environment.