# MINIX cp - File Copy Utility

## Overview

The `cp` utility is a comprehensive file copying program that serves as one of the most fundamental tools in the MINIX 3 operating system. It provides sophisticated file copying capabilities including recursive directory copying, preservation of file attributes, and various safety mechanisms to prevent accidental data loss.

## Purpose and Functionality

cp performs advanced file copying operations with features including:
1. **Basic File Copying**: Copy individual files to new locations
2. **Directory Tree Copying**: Recursive copying of entire directory hierarchies
3. **Attribute Preservation**: Maintain file permissions, timestamps, and ownership
4. **Link Handling**: Various strategies for copying symbolic and hard links
5. **Safety Features**: Interactive mode, backup creation, and overwrite protection

## Directory Structure

```
bin/cp/
├── cp.c           # Main implementation source
├── cp.1           # Manual page documentation
├── extern.h       # External declarations and shared structures
├── utils.c        # Utility functions for copying operations
├── Makefile       # Build configuration
└── obj/           # Build output directory
```

## Source Files

### cp.c - Main Implementation
The core implementation containing the main logic and command-line processing:

- **Lines**: ~400 lines of C code
- **Architecture**: NetBSD-derived implementation
- **License**: BSD 3-clause (University of California/Berkeley)

**Key Functions:**
- `main()`: Command-line parsing and overall program flow control
- `copy_file()`: Core file copying logic with FTS (File Tree Specification) integration
- `copy_special()`: Handle special files (devices, FIFOs)
- `copy_fifo()`: Special handling for named pipes
- `copy_link()`: Symbolic and hard link copying strategies
- `setfile()`: Apply file attributes (permissions, ownership, timestamps)

**Command-Line Options:**
- `-R`: Recursive copying of directories
- `-r`: Recursive copying (deprecated, use -R)
- `-H`: Follow symbolic links specified on command line
- `-L`: Follow all symbolic links
- `-P`: Never follow symbolic links (default)
- `-f`: Force overwrite existing files
- `-i`: Interactive mode - prompt before overwriting
- `-l`: Create hard links instead of copying
- `-N`: Don't preserve file flags
- `-n`: Don't overwrite existing files
- `-p`: Preserve file attributes (mode, timestamps, ownership)
- `-v`: Verbose output

### extern.h - Shared Declarations
Header file defining shared structures and global variables:

**Key Structures:**
```c
typedef struct {
    char *p_end;                    /* pointer to NULL at end of path */
    char *target_end;               /* pointer to end of target base */
    char p_path[MAXPATHLEN + 1];    /* pointer to the start of a path */
} PATH_T;
```

**Global Variables:**
- `to`: Target path structure for destination handling
- `myuid`: Current user ID for permission checking
- Various flag variables for command-line options
- `myumask`: Current file creation mask

### utils.c - Utility Functions
Supporting functions for file operations and attribute management:

**Core Functions:**
- `setfile()`: Apply file attributes (permissions, ownership, timestamps)
- `set_utimes()`: Set file access and modification times
- `copy_file()`: Main file copying implementation with memory mapping support
- `copy_special()`: Handle special files (devices, FIFOs)
- `copy_fifo()`: Named pipe copying with content preservation

**Advanced Features:**
- Memory-mapped file I/O for efficient copying
- Extended attribute preservation
- Signal handling for progress reporting
- Atomic file operations where possible

### cp.1 - Manual Page
Comprehensive documentation covering:
- Complete option descriptions with examples
- File attribute preservation behavior
- Symbolic link handling strategies
- Recursive copying behavior
- Exit status codes
- Standards compliance information

## Build Configuration

### Makefile
```makefile
PROG=    cp
.include <bsd.prog.mk>
```

**Build Process:**
- Standard MINIX BSD make infrastructure
- Links against standard C library (libc)
- FTS library support for directory tree traversal
- Cross-platform compilation for all MINIX architectures

## Implementation Details

### File Tree Traversal (FTS)
The cp utility uses the FTS (File Tree Specification) library for efficient directory tree traversal:
- Handles circular link detection
- Provides depth-first directory traversal
- Supports various traversal options and filters
- Efficient memory usage for large directory trees

### Memory Management
- Memory-mapped file I/O for large file copying
- Dynamic buffer allocation for optimal performance
- Careful memory cleanup in error conditions
- Support for sparse file detection and handling

### Error Handling
- Comprehensive error checking at each operation step
- Graceful handling of permission denied scenarios
- Proper cleanup of partially copied files
- Detailed error messages with file path context

### Attribute Preservation
- File permission bits (mode)
- Access and modification timestamps
- Ownership information (when permitted)
- Extended attributes (where supported)
- File flags and special attributes

## MINIX Integration

### System Dependencies
- VFS (Virtual File System) for file operations
- Standard C library file I/O functions
- MINIX process and permission system
- Signal handling for user interaction

### Service Interaction
- Uses VFS for all file system operations
- Respects MINIX permission and security model
- Integrates with MINIX user/group database
- Follows MINIX file system semantics

## RISC-V 64-bit Considerations

### Architecture-Specific Notes
- No RISC-V specific code required (portable implementation)
- Memory-mapped I/O benefits from RISC-V's regular memory model
- Efficient buffer operations on RISC-V load/store architecture
- Large file support compatible with RISC-V 64-bit addressing

### Performance Characteristics
- Memory mapping provides excellent performance on RISC-V
- Large buffer operations leverage RISC-V's efficient memory access
- Directory traversal scales well with RISC-V's processing power
- No alignment issues on RISC-V's regular instruction set

### Cross-Compilation
- Builds cleanly with riscv64-unknown-elf-gcc
- No architecture-specific compilation flags required
- Standard NetBSD portability ensures RISC-V compatibility

## Advanced Features

### Sparse File Handling
- Detection and preservation of file holes
- Efficient copying of sparse files
- Maintains file size while optimizing storage

### Interactive Mode
- User confirmation for overwrite operations
- Graceful handling of user responses
- Proper cleanup on interruption

### Backup Creation
- Optional backup of existing destination files
- Configurable backup suffixes
- Integration with backup directory schemes

### Progress Reporting
- Verbose mode for detailed operation tracking
- Signal-based progress reporting
- Comprehensive exit status reporting

## Testing and Validation

### Test Coverage
- Basic file copying scenarios
- Recursive directory copying
- Symbolic link handling
- Attribute preservation verification
- Error condition handling
- Large file operations
- Special file handling

### Integration Tests
- Part of MINIX base system test suite
- Cross-architecture compatibility testing
- Standards compliance validation
- Performance regression testing

## Usage Examples

```bash
# Basic file copy
cp source.txt destination.txt

# Recursive directory copy
cp -R source_dir/ dest_dir/

# Preserve attributes
cp -p important_file backup_file

# Interactive copy with confirmation
cp -i existing_file new_file

# Verbose recursive copy
cp -vR project/ backup/

# Copy with symbolic link preservation
cp -P linked_dir/ copy_dir/

# Create hard link instead of copy
cp -l large_file hardlink_file
```

## Standards Compliance

The MINIX cp implementation follows:
- IEEE Std 1003.1 (POSIX.1)
- Single UNIX Specification (SUS)
- NetBSD compatibility standards
- BSD historical behavior for compatibility

## Security Considerations

### Permission Handling
- Respects MINIX permission model
- Proper handling of setuid/setgid bits
- Careful preservation of file ownership
- Security-conscious temporary file handling

### Safe Defaults
- Interactive mode for potentially destructive operations
- Backup creation options
- Non-overwriting behavior options
- Comprehensive error checking

## Development Notes

### Code Architecture
- Clean separation between main logic and utility functions
- Modular design for maintainability
- Consistent error handling patterns
- Comprehensive documentation

### Performance Optimizations
- Memory-mapped I/O for large files
- Efficient directory traversal algorithms
- Optimized buffer sizes for different scenarios
- Minimal system call overhead

This implementation provides a full-featured, standards-compliant cp utility that handles complex copying scenarios while maintaining the reliability and security expected in the MINIX 3 microkernel environment. The modular design and comprehensive feature set make it suitable for both interactive use and system administration tasks across all MINIX architectures, including RISC-V 64-bit.