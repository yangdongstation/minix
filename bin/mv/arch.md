# MINIX mv - File Move/Rename Utility

## Overview

The `mv` utility is a fundamental file system manipulation tool that provides file renaming and moving capabilities in MINIX 3. It combines the functionality of renaming files within a directory and moving files between directories, with sophisticated handling of edge cases and safety features to prevent accidental data loss.

## Purpose and Functionality

mv performs essential file system operations including:
1. **File Renaming**: Change file names within the same directory
2. **File Moving**: Transfer files between different directories
3. **Directory Operations**: Move and rename entire directory structures
4. **Batch Operations**: Handle multiple source files with safety checks
5. **Cross-Device Operations**: Handle moves between different file systems

The utility implements intelligent behavior to optimize operations, using simple rename operations when possible and falling back to copy-and-delete for cross-device moves.

## Directory Structure

```
bin/mv/
├── mv.c           # Main implementation source
├── mv.1           # Manual page documentation
├── pathnames.h    # Path definitions for utility dependencies
├── Makefile       # Build configuration
└── obj/           # Build output directory
```

## Source Files

### mv.c - Main Implementation
The complete implementation of the mv utility:

- **Lines**: ~300 lines of C code
- **Architecture**: NetBSD-derived implementation
- **License**: BSD 3-clause (University of California/Berkeley)

**Key Functions:**
- `main()`: Command-line parsing and operation dispatch
- `copy()`: Fallback copy implementation for cross-device operations
- `usage()`: Help text and error message display
- File existence checking and collision handling
- Permission and safety validation

**Command-Line Options:**
- `-f`: Force operation, overwrite existing files without prompting
- `-i`: Interactive mode, prompt before overwriting existing files
- `-n`: Never overwrite existing files (no-clobber mode)
- `-v`: Verbose mode, display operations as they are performed

### pathnames.h - Utility Dependencies
Defines paths to dependent utilities for fallback operations:

```c
#define _PATH_RM    "/bin/rm"    /* Remove utility path */
#define _PATH_CP    "/bin/cp"    /* Copy utility path */
```

This header enables mv to invoke rm and cp utilities when performing cross-device moves that require copy-and-delete operations.

### mv.1 - Manual Page
Comprehensive documentation including:
- Complete option descriptions
- Operation semantics and edge cases
- Examples of common usage patterns
- Exit status codes
- Standards compliance information

## Build Configuration

### Makefile
```makefile
PROG=    mv
.include <bsd.prog.mk>
```

**Build Process:**
- Standard MINIX BSD make infrastructure
- Links against standard C library (libc)
- No special dependencies or compilation flags
- Cross-platform compilation support

## Implementation Details

### Operation Logic
The mv utility implements a sophisticated decision tree for determining the optimal operation method:

1. **Same Device Rename**: When source and destination are on the same file system, use the efficient `rename()` system call
2. **Cross-Device Operations**: When moving between file systems, fall back to copy-and-delete strategy
3. **Directory Handling**: Special logic for moving directories across device boundaries
4. **Batch Operations**: Handle multiple source files with consistent safety checks

### Safety Mechanisms
Multiple layers of protection against accidental data loss:
- **Interactive Mode**: Prompts before overwriting existing files
- **No-Clobber Mode**: Prevents any overwriting of existing files
- **Permission Checks**: Validates write permissions before operations
- **Atomic Operations**: Uses atomic rename where possible
- **Error Recovery**: Proper cleanup on operation failure

### Cross-Device Operations
When moving files between different file systems:
- Invokes cp utility to copy file contents and attributes
- Uses rm utility to remove source files after successful copy
- Maintains original file permissions and timestamps
- Handles special files appropriately

### Error Handling
Comprehensive error detection and reporting:
- File existence validation
- Permission checking
- Device space verification
- Atomic operation failure handling
- Graceful degradation for unsupported operations

## MINIX Integration

### System Dependencies
- MINIX VFS (Virtual File System) for rename operations
- Standard C library file system functions
- Process execution for cp/rm fallback operations
- MINIX permission and security model

### Service Interaction
- Uses VFS for file system operations
- Integrates with MINIX process management for utility invocation
- Respects MINIX file permission semantics
- Handles MINIX-specific file system features

## RISC-V 64-bit Considerations

### Architecture-Specific Notes
- No RISC-V specific code required (portable implementation)
- Efficient system call interface on RISC-V architecture
- Large file support with 64-bit file system operations
- Cross-device operations work seamlessly on RISC-V

### Performance Characteristics
- Rename operations highly efficient on RISC-V
- Copy operations benefit from RISC-V's memory performance
- Process spawning for fallback operations efficient on RISC-V
- No alignment or memory model issues on RISC-V

### Cross-Compilation
- Builds successfully with riscv64-unknown-elf-gcc
- No architecture-specific compilation flags needed
- Standard NetBSD portability ensures RISC-V compatibility

## Advanced Features

### Intelligent Operation Selection
The utility automatically selects the most efficient operation method:
- Prefer atomic rename operations when possible
- Fall back to copy-and-delete only when necessary
- Optimize for the common case of same-directory renames

### Batch Operation Handling
Sophisticated handling of multiple source files:
- Consistent application of safety options across all operations
- Proper error handling for partial failures
- Atomic behavior where possible

### Directory Tree Operations
Special handling for directory moves:
- Validation of directory structure integrity
- Prevention of self-moves (moving directory into itself)
- Cross-device directory tree copying
- Preservation of directory attributes

### Symbolic Link Handling
Proper handling of symbolic links:
- Option to follow or preserve symbolic links
- Prevention of circular link creation
- Cross-device link handling

## Testing and Validation

### Test Coverage
- Basic file rename operations
- Cross-device move operations
- Directory moving and renaming
- Batch operation handling
- Error condition responses
- Permission and safety validation

### Integration Tests
- Part of MINIX base system test suite
- Cross-architecture compatibility testing
- Fallback operation verification
- Standards compliance validation

## Usage Examples

```bash
# Simple file rename
mv oldname.txt newname.txt

# Move file to different directory
mv file.txt /path/to/directory/

# Move and rename
mv oldfile.txt /new/path/newfile.txt

# Interactive mode with confirmation
mv -i existing_file.txt new_file.txt

# Verbose operation
mv -v source.txt destination.txt

# Never overwrite existing files
mv -n source.txt existing_file.txt

# Force overwrite without prompting
mv -f source.txt existing_file.txt

# Rename directory
mv old_directory/ new_directory/

# Move multiple files to directory
mv file1.txt file2.txt file3.txt target_directory/
```

## Standards Compliance

The MINIX mv implementation follows:
- IEEE Std 1003.1 (POSIX.1)
- Single UNIX Specification (SUS)
- NetBSD compatibility standards
- BSD historical behavior for compatibility

## Error Handling and Exit Codes

### Exit Status
- **0**: All operations successful
- **1**: One or more operations failed
- **>1**: Serious error occurred

### Common Error Conditions
- Permission denied
- File not found
- Directory not empty
- Cross-device link errors
- Invalid arguments

## Security Considerations

### Permission Validation
- Validates write permissions on source and destination
- Respects MINIX security model
- Proper handling of setuid/setgid files
- Careful validation of symbolic link operations

### Safe Defaults
- Interactive mode available for safety
- Non-destructive options as alternatives
- Comprehensive error checking
- Atomic operations where possible

This implementation provides a robust, user-friendly mv utility that handles the complexities of file system operations while maintaining safety and reliability. The intelligent operation selection and comprehensive error handling make it suitable for both casual use and system administration tasks across all MINIX architectures, including RISC-V 64-bit.