# MINIX Virtual File System (VFS) Server Architecture

This directory contains the Virtual File System server implementation, which provides a unified interface to all file systems in MINIX. The VFS handles file operations, pathname resolution, file descriptors, and coordinates with various file system implementations and device drivers.

## Core VFS Infrastructure

### Server Framework
- **main.c**: VFS server main program
  - Server initialization and setup
  - Message loop and dispatch
  - Service registration
  - Main server state management
  - Error handling and recovery

- **fs.h**: VFS server definitions
  - VFS-specific constants and macros
  - Server configuration parameters
  - VFS service definitions
  - Internal data structure definitions

- **const.h**: VFS constants
  - File system limits and parameters
  - Message type definitions
  - Return code definitions
  - Buffer size constants
  - Path length limits

### File Process Management
- **fproc.h**: File process table structure
  - Per-process file system state
  - File descriptor information
  - Current working directory
  - Root directory tracking
  - File creation mask
  - User and group identification

- **glo.h**: Global variables
  - VFS global state
  - File process table
  - System-wide counters
  - Configuration parameters
  - Shared state information

### File Descriptor Management
- **filedes.c**: File descriptor operations
  - File descriptor allocation
  - Descriptor table management
  - Dup() and dup2() implementation
  - Close-on-exec handling
  - File descriptor inheritance
  - Descriptor reference counting

- **file.h**: File structure definitions
  - File object structure
  - File state information
  - Position tracking
  - Access mode storage
  - File system specific data

## File System Operations

### Basic File I/O
- **open.c**: File opening
  - Pathname resolution
  - File creation logic
  - Access permission checking
  - File system selection
  - Device special file handling

- **read.c**: File reading
  - Read system call implementation
  - Data buffering
  - Position tracking
  - End-of-file handling
  - Error condition checking

- **write.c**: File writing
  - Write system call implementation
  - Data synchronization
  - Space allocation coordination
  - Write permission checking
  - Append mode handling

### File Metadata
- **stat.c**: File status operations
  - stat() and fstat() implementation
  - File information retrieval
  - Permission and ownership data
  - Size and timestamp information
  - Device identification

- **protect.c**: File protection
  - chmod() and fchmod() implementation
  - chown() and fchown() implementation
  - Permission checking logic
  - Access control validation
  - Ownership verification

### File Manipulation
- **link.c**: File linking operations
  - Hard link creation
  - Symbolic link creation
  - Link count management
  - Cross-device link prevention
  - Directory link restrictions

- **directory.c**: Directory operations
  - mkdir() and rmdir() implementation
  - Directory entry creation
  - Empty directory verification
  - Parent directory updates

- **mount.c**: File system mounting
  - Mount system call implementation
  - File system type detection
  - Mount point validation
  - Device mounting coordination
  - Unmount operations

## Device and Special Files

### Device File Handling
- **device.c**: Device special files
  - Device file identification
  - Major/minor device numbers
  - Device driver coordination
  - Device access permissions
  - Special file creation

### Block Devices
- **bdev.c**: Block device interface
  - Block device registration
  - Block I/O request handling
  - Buffer cache coordination
  - Block size management
  - Device driver communication

### Character Devices
- **cdev.c**: Character device interface
  - Character device registration
  - Character I/O operations
  - Line discipline support
  - Terminal device handling
  - Device driver communication

## Advanced File System Features

### File Locking
- **lock.c**: File locking
  - Advisory locking (flock)
  - Record locking (fcntl)
  - Lock conflict resolution
  - Deadlock detection
  - Lock inheritance

### Select and Poll
- **select.c**: I/O multiplexing
  - select() system call implementation
  - File descriptor set management
  - Event notification coordination
  - Timeout handling
  - Multi-device polling

### Symbolic Links
- **readlink.c**: Symbolic link reading
  - readlink() system call
  - Symbolic link resolution
  - Link content retrieval
  - Path length validation

### File System Information
- **statvfs.c**: File system statistics
  - File system capacity information
  - Free space reporting
  - Block size information
  - File system type identification

## File System Coordination

### Device Mapping
- **dmap.c/dmap.h**: Device mapping table
  - Device number to driver mapping
  - Device registration
  - Driver lookup mechanisms
  - Dynamic device management
  - Block and character device tables

### Communication
- **comm.c**: Inter-server communication
  - Message passing to file systems
  - Driver communication coordination
  - Reply message handling
  - Error propagation
  - Timeout management

### Utility Functions
- **utility.c**: VFS utility functions
  - Pathname manipulation
  - Permission checking helpers
  - Memory allocation utilities
  - String processing functions
  - Error handling routines

- **path.c**: Pathname resolution
  - Absolute and relative path handling
  - Symbolic link resolution
  - ".." and "." handling
  - Root directory boundary
  - Path component parsing

## Execution Support

### Program Execution
- **exec.c**: Program execution support
  - exec() family system calls
  - Binary format detection
  - Interpreter script handling
  - Argument and environment setup
  - Memory layout preparation
  - File descriptor handling

### Core Dump Generation
- **coredump.c**: Core dump creation
  - Process memory dumping
  - Core file format generation
  - Signal context preservation
  - Debugging information

## Locking and Synchronization

### VFS Locking
- **lock.c**: VFS internal locking
  - VFS data structure protection
  - Concurrent access prevention
  - Deadlock avoidance
  - Lock ordering protocols

### File System Synchronization
- **sync.c**: File system synchronization
  - sync() system call
  - Buffer cache flushing
  - Metadata synchronization
  - Scheduled synchronization

## Special File Systems

### Pipe File System
- **pipe.c**: Pipe implementation
  - Anonymous pipe creation
  - Pipe buffer management
  - Reader/writer coordination
  - Pipe capacity enforcement
  - Blocking I/O handling

### Proc File System
- **procfs.c**: /proc file system
  - Process information export
  - System statistics
  - Kernel parameter access
  - Dynamic file generation

## Code Coverage Support

### Profiling
- **gcov.c**: Code coverage support
  - GCC coverage data collection
  - Coverage file generation
  - Statistical analysis support

## File System Driver Interface

### Driver Communication
The VFS coordinates with multiple file system drivers:
- **MINIX File System**: Native MINIX file system
- **ISO 9660**: CD-ROM file system
- **MS-DOS**: FAT file system support
- **Network File Systems**: NFS client support
- **Virtual File Systems**: procfs, pipefs

### Message Protocol
VFS uses structured message passing:
- **Request Messages**: File operation requests
- **Reply Messages**: Operation results
- **Notification Messages**: Asynchronous events
- **Error Messages**: Failure reporting

## Integration with MINIX System

### Inter-Server Communication
- **PM Coordination**: Process state synchronization
- **VM Collaboration**: Memory mapping operations
- **Driver Communication**: Device I/O coordination
- **RS Registration**: Service monitoring

### Security Model
- **Access Control**: Permission validation
- **Capability Checking**: Resource access authorization
- **User/Group Validation**: Identity verification
- **Audit Trail**: File operation logging

The VFS server is central to MINIX's file system architecture, providing a unified interface while coordinating multiple file system implementations and maintaining system security and consistency.