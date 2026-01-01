# MINIX Filesystem Check and Repair Utility (fsck)

## Overview

The `fsck` utility is a critical system administration tool for checking and repairing filesystem consistency in MINIX systems. It provides comprehensive filesystem validation, corruption detection, and automated repair capabilities. This utility is essential for maintaining filesystem integrity, recovering from system crashes, and ensuring data reliability across various filesystem types supported by MINIX.

## Architecture

### Core Functionality
- **Filesystem Validation**: Comprehensive consistency checking
- **Corruption Detection**: Identify and report filesystem errors
- **Automated Repair**: Fix common filesystem problems
- **Multi-Filesystem Support**: Handle different filesystem types
- **Pre-Flight Checking**: Fast verification for boot sequences
- **Interactive Mode**: User-guided repair operations
- **Batch Processing**: Automated repair with minimal interaction

### MINIX Integration
- Communicates with VFS server for filesystem operations
- Uses MINIX message-passing for system calls
- Integrates with PM server for process management
- Supports MINIX-specific filesystem features
- Boot-time integration for filesystem recovery

## Source Files

### Main Implementation
- **`fsck.c`** (13,209 lines)
  - Main program entry point and command dispatcher
  - Filesystem type detection and dispatch
  - Overall checking coordination and control
  - User interface and interaction handling
  - Error reporting and logging functionality
  - Integration with MINIX system services
  - Boot-time checking logic
  - Signal handling and cleanup

### Filesystem Utilities
- **`fsutil.c`** (6,479 lines)
  - Common filesystem utility functions
  - Block device I/O operations
  - Filesystem metadata manipulation
  - Directory traversal utilities
  - Inode management functions
  - Block allocation and deallocation
  - Filesystem statistics and reporting

- **`fsutil.h`** (2,323 lines)
  - Filesystem utility function declarations
  - Common data structure definitions
  - Filesystem operation constants
  - Error code definitions
  - Utility macro definitions

### Partition Utilities
- **`partutil.c`** (5,719 lines)
  - Partition table management
  - Disk label processing
  - Partition validation and repair
  - Boot block verification
  - Partition geometry calculations
  - Multi-disk filesystem checking

- **`partutil.h`** (1,840 lines)
  - Partition utility declarations
  - Partition table structure definitions
  - Disk geometry constants
  - Partition type definitions

### Progress and Status
- **`preen.c`** (9,373 lines)
  - Pre-flight checking mode ("preen" mode)
  - Fast filesystem verification
  - Boot-time checking optimization
  - Minimal interaction repair
  - Automatic error correction
  - System startup integration

- **`progress.c`** (4,564 lines)
  - Progress reporting and display
  - Percentage completion tracking
  - Phase-based progress indication
  - User feedback mechanisms
  - Time estimation and remaining

- **`progress.h`** (1,899 lines)
  - Progress reporting interface
  - Progress callback definitions
  - Display formatting constants

### Configuration and Constants
- **`pathnames.h`** (1,444 lines)
  - System path definitions
  - Configuration file locations
  - Default filesystem paths
  - Device node paths

- **`exitvalues.h`** (1,746 lines)
  - Exit status code definitions
  - Error condition constants
  - Return value specifications
  - Status reporting standards

### Manual and Documentation
- **`fsck.8`** (5,297 lines)
  - Comprehensive manual page
  - Usage examples and options
  - Filesystem-specific information
  - Error message explanations
  - Recovery procedures

## Build Configuration

### Makefile Structure
- **`Makefile`** (202 lines)
  - Standard MINIX build configuration
  - Source file compilation rules
  - Library dependencies and linking
  - Installation targets
  - Cross-compilation support

### Dependencies
- **System Libraries**: libc, libutil, libutil
- **Filesystem Libraries**: libfs, libminixfs
- **MINIX Libraries**: libminix, libsys, libtimers
- **Header Files**: sys/types.h, sys/fs.h, minix/fs.h

### Build Requirements
- C compiler with C99 support
- MINIX cross-compilation toolchain
- Filesystem type definitions
- Block device interface headers

## MINIX System Integration

### Server Communication
```c
// Example: Filesystem check request
message m;
m.m_type = VFS_CHECK_FS;
m.VFS_CHECK_DEV = device_number;
m.VFS_CHECK_FLAGS = check_flags;
m.VFS_CHECK_MODE = check_mode;
_sendrec(VFS_PROC_NR, &m);
```

### Service Dependencies
- **VFS Server**: Primary filesystem interface
- **PM Server**: Process management and privileges
- **DS Server**: Service discovery and registration
- **MEM Server**: Memory management for large operations

### Filesystem Type Support
```c
// Filesystem type detection
struct fs_info {
    const char *name;
    int (*check)(const char *device, int flags);
    int (*repair)(const char *device, int flags);
    const char *description;
};
```

## Filesystem Checking Process

### Check Phases
1. **Pre-Flight Check**: Quick validation for boot sequences
2. **Superblock Verification**: Filesystem metadata validation
3. **Inode Table Check**: File metadata consistency
4. **Directory Structure**: Path and linkage validation
5. **Block Allocation**: Free block list verification
6. **Reference Counts**: Link count consistency
7. **Permission Checks**: Access control validation
8. **Cleanup and Summary**: Final validation and reporting

### Error Detection Categories
- **Superblock Errors**: Invalid filesystem parameters
- **Inode Corruption**: Damaged file metadata
- **Directory Errors**: Broken directory structures
- **Block Allocation**: Lost blocks or allocation conflicts
- **Link Counts**: Incorrect hard link counts
- **Permission Errors**: Invalid access permissions
- **Cross-Reference**: Inconsistent filesystem references

### Repair Strategies
- **Automatic Repair**: Fix common errors without prompting
- **Interactive Repair**: User confirmation for changes
- **Conservative Repair**: Minimal changes to preserve data
- **Aggressive Repair**: Comprehensive error correction
- **Read-Only Mode**: Check without modifications

## Filesystem Type Support

### MINIX Filesystem
- **minixfs**: Native MINIX filesystem
- **minixfs2**: MINIX filesystem version 2
- **v1fs**: Original MINIX filesystem
- **v3fs**: MINIX version 3 filesystem

### Foreign Filesystem Support
- **ext2**: Extended filesystem (via fsck_ext2fs)
- **msdos**: MS-DOS FAT filesystem
- **iso9660**: CD-ROM filesystem
- **udf**: Universal Disk Format

### Filesystem Features
| Feature | MINIX FS | EXT2 | FAT | Notes |
|---------|----------|------|-----|-------|
| Block Checking | ✓ Full | ✓ Full | ✓ Basic | Platform-specific |
| Inode Validation | ✓ Full | ✓ Full | ✗ N/A | Filesystem-specific |
| Directory Repair | ✓ Full | ✓ Full | ✓ Limited | Depends on complexity |
| Permission Fix | ✓ Full | ✓ Full | ✗ N/A | Unix permissions |
| Link Count Repair | ✓ Full | ✓ Full | ✗ N/A | Hard links only |

## Boot-Time Integration

### Pre-Flight Mode (Preen)
- **Fast Verification**: Quick filesystem checks
- **Automatic Repair**: Fix obvious errors
- **Minimal Interaction**: Reduce boot delays
- **Error Aggregation**: Collect and report issues
- **Fallback Strategy**: Full check if preen fails

### Boot Sequence Integration
```bash
# Typical boot sequence
fsck -p /dev/hd0     # Preen root filesystem
mount /dev/hd0 /     # Mount root if clean
fsck -y /dev/hd0     # Force repair if dirty
```

### Recovery Mode
- **Single-User Mode**: Filesystem recovery
- **Manual Intervention**: Administrator control
- **Emergency Repair**: Critical error handling
- **Backup Integration**: Preserve data before repair

## RISC-V 64-bit Platform Considerations

### Architecture Independence
- Portable C implementation
- No assembly code dependencies
- Platform-agnostic algorithms
- Cross-architecture compatibility

### RISC-V Specific Optimizations
- Memory-aligned data structures
- Efficient block I/O operations
- Optimized checksum calculations
- Cache-friendly data access

### Block Device Interface
- Standard MINIX device interface
- Platform-independent I/O operations
- Efficient buffer management
- Memory-mapped device support

### Performance Considerations
- Large filesystem support
- Efficient memory usage
- Fast error detection
- Minimal CPU overhead

## Error Handling and Recovery

### Error Classification
- **Correctable Errors**: Minor inconsistencies
- **Serious Errors**: Data integrity issues
- **Fatal Errors**: Unrecoverable corruption
- **Warning Conditions**: Potential problems

### Recovery Mechanisms
- **Automatic Correction**: Common error fixes
- **User Confirmation**: Critical changes
- **Data Preservation**: Minimize data loss
- **Backup Creation**: Preserve original state
- **Logging**: Detailed error recording

### Safety Features
- **Read-Only Mode**: Non-destructive checking
- **Backup Integration**: Preserve user data
- **Transaction Logging**: Track all changes
- **Rollback Capability**: Undo modifications
- **Verification**: Confirm repairs succeeded

## Security Considerations

### Access Control
- Privilege checking for device access
- Filesystem permission validation
- Secure device node handling
- Audit logging for operations

### Data Protection
- Secure error handling
- Protected memory operations
- Safe string handling
- Buffer overflow prevention

### Input Validation
- Device path validation
- Filesystem type verification
- Parameter range checking
- Command injection prevention

## Performance Optimization

### Check Speed Optimization
- Parallel checking algorithms
- Efficient data structures
- Memory caching strategies
- I/O optimization techniques

### Large Filesystem Support
- Incremental checking
- Memory-efficient algorithms
- Progress reporting
- Interruptible operations

### Resource Management
- Memory allocation control
- CPU usage optimization
- I/O bandwidth management
- System responsiveness

## Usage Examples

### Basic Checking
```bash
# Check filesystem (read-only)
fsck /dev/hd0

# Check with automatic repair
fsck -y /dev/hd0

# Preen mode (boot-time)
fsck -p

# Interactive repair
fsck -i /dev/hd0
```

### Advanced Usage
```bash
# Force checking
fsck -f /dev/hd0

# Verbose output
fsck -v /dev/hd0

# Check specific filesystem type
fsck -t minix /dev/hd0

# Multiple filesystems
fsck -A
```

### Emergency Recovery
```bash
# Emergency repair mode
fsck -b 8193 /dev/hd0

# Skip certain checks
fsck -x /dev/hd0

# Debug mode
fsck -d /dev/hd0
```

## Testing and Validation

### Functional Testing
- Filesystem corruption simulation
- Repair verification
- Multi-filesystem support
- Error condition testing

### Performance Testing
- Large filesystem benchmarks
- Concurrent access testing
- Memory usage validation
- I/O performance measurement

### Platform Testing
- RISC-V 64-bit validation
- Cross-architecture testing
- Device driver integration
- Boot sequence verification

## Maintenance and Updates

### NetBSD Synchronization
- Regular updates from NetBSD-8
- Security patches and fixes
- Filesystem enhancements
- Performance improvements

### MINIX-Specific Enhancements
- Message-based system integration
- Service-oriented architecture
- Enhanced reliability features
- Microkernel optimization

### Filesystem Evolution
- New filesystem support
- Enhanced repair algorithms
- Improved error detection
- Better user experience

The `fsck` utility provides comprehensive filesystem checking and repair capabilities essential for MINIX system reliability. Its robust error detection, multiple filesystem support, and tight integration with MINIX system services make it a critical component for filesystem maintenance and data recovery in the microkernel environment.