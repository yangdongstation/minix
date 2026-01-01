# MINIX rm - File Removal Utility

## Overview

The `rm` utility is a critical file system manipulation tool that provides file and directory removal capabilities in MINIX 3. As one of the most potentially destructive UNIX commands, rm implements multiple safety mechanisms and confirmation prompts to prevent accidental data loss while providing efficient file deletion functionality.

## Purpose and Functionality

rm performs file system cleanup operations including:
1. **File Deletion**: Remove individual files and multiple files
2. **Directory Tree Removal**: Recursively delete directory hierarchies
3. **Interactive Removal**: Confirm each deletion operation
4. **Force Removal**: Override permission restrictions and safety checks
5. **Selective Deletion**: Remove files matching specific patterns or criteria

The utility implements sophisticated safety features including interactive confirmation, recursive deletion warnings, and dry-run capabilities to prevent accidental data loss.

## Directory Structure

```
bin/rm/
├── rm.c           # Main implementation source
├── rm.1           # Manual page documentation
├── Makefile       # Build configuration
└── obj/           # Build output directory
```

## Source Files

### rm.c - Main Implementation
The comprehensive implementation of the rm utility:

- **Lines**: ~400 lines of C code
- **Architecture**: NetBSD-derived implementation
- **License**: BSD 3-clause (University of California/Berkeley)

**Key Functions:**
- `main()`: Command-line parsing and overall program flow
- `remove_file()`: Core file removal logic with safety checks
- `remove_dir()`: Recursive directory removal implementation
- `check()`: Safety validation and confirmation prompts
- `usage()`: Help text and usage information
- `sigint_handler()`: Signal handling for graceful interruption

**Command-Line Options:**
- `-f`: Force removal, ignore nonexistent files, never prompt
- `-i`: Interactive mode, prompt before each removal
- `-I`: Prompt once before removing more than three files or recursively
- `-P`: Overwrite regular files before removing (secure deletion)
- `-R`: Recursive removal of directories and their contents
- `-r`: Recursive removal (equivalent to -R)
- `-v`: Verbose mode, explain what is being done
- `-d`: Remove empty directories (no recursive deletion required)
- `-x`: Don't cross device boundaries when removing recursively

### rm.1 - Manual Page
Comprehensive documentation covering:
- Complete option descriptions with safety implications
- Examples of safe and dangerous usage patterns
- Exit status codes and error conditions
- Security considerations and best practices
- Standards compliance information

## Build Configuration

### Makefile
```makefile
PROG=    rm
.include <bsd.prog.mk>
```

**Build Process:**
- Standard MINIX BSD make infrastructure
- Links against standard C library (libc)
- No special dependencies or compilation flags
- Cross-platform compilation support for all MINIX architectures

## Implementation Details

### Safety Mechanisms
Multiple layers of protection against accidental file deletion:

**Interactive Confirmation:**
- `-i` option prompts before each file removal
- `-I` option prompts for bulk operations (>3 files or recursive)
- Clear indication of potentially dangerous operations
- Confirmation of recursive directory deletion

**Permission and Access Control:**
- Validation of file permissions before removal
- Proper handling of read-only files
- Setuid/setgid file warnings
- Directory write permission verification

**Recursive Deletion Safety:**
- Explicit `-R` or `-r` flag required for directory removal
- Confirmation prompts for recursive operations
- Prevention of accidental root directory deletion
- Symbolic link traversal protection

### File Removal Strategies
Different approaches based on file type and options:

**Regular Files:**
- Standard `unlink()` system call
- Optional secure overwrite with `-P` flag
- Permission checking and error handling
- Verbose output when requested

**Directories:**
- Recursive traversal with FTS library
- Empty directory validation for `-d` option
- Cross-device boundary handling with `-x` option
- Comprehensive error reporting

**Special Files:**
- Device file handling
- Named pipe (FIFO) removal
- Socket file cleanup
- Symbolic link removal (not following links)

### Signal Handling
Robust signal handling for user interruption:
- SIGINT (Ctrl+C) handling for graceful termination
- Partial operation cleanup
- Consistent state maintenance
- Proper error reporting on interruption

## MINIX Integration

### System Dependencies
- MINIX VFS (Virtual File System) for file operations
- Standard C library file system functions
- FTS library for directory tree traversal
- MINIX permission and security model

### Service Interaction
- Uses VFS for all file system operations
- Integrates with MINIX permission system
- Respects MINIX file system semantics
- Handles MINIX-specific file types

## RISC-V 64-bit Considerations

### Architecture-Specific Notes
- No RISC-V specific code required (portable implementation)
- Efficient system call interface on RISC-V architecture
- Large file support with 64-bit file system operations
- Recursive operations scale well with RISC-V performance

### Performance Characteristics
- File deletion operations efficient on RISC-V
- Directory tree traversal benefits from RISC-V memory architecture
- Signal handling optimized for RISC-V interrupt model
- No alignment or memory model issues on RISC-V

### Cross-Compilation
- Builds successfully with riscv64-unknown-elf-gcc
- No architecture-specific compilation flags needed
- Standard NetBSD portability ensures RISC-V compatibility

## Advanced Features

### Secure Deletion
The `-P` option provides secure file deletion:
- Overwrites file contents before removal
- Multiple overwrite passes for security
- Applies only to regular files
- Prevents simple data recovery

### Verbose Operations
The `-v` option provides detailed operation reporting:
- Lists each file as it is removed
- Shows recursive directory operations
- Reports special file handling
- Provides operation completion feedback

### Cross-Device Restrictions
The `-x` option prevents crossing device boundaries:
- Stays within the same file system
- Prevents accidental removal across mounted file systems
- Useful for targeted cleanup operations
- Maintains operation predictability

### Empty Directory Removal
The `-d` option enables removal of empty directories:
- No recursive deletion required
- Faster than recursive removal for empty directories
- Safer for directory cleanup scripts
- Validates directory emptiness before removal

## Testing and Validation

### Test Coverage
- Basic file removal operations
- Recursive directory removal
- Interactive confirmation prompts
- Error condition handling
- Signal interruption scenarios
- Permission and access control
- Special file type handling

### Integration Tests
- Part of MINIX base system test suite
- Cross-architecture compatibility testing
- Safety mechanism validation
- Standards compliance verification

## Usage Examples

```bash
# Remove a single file
rm unwanted_file.txt

# Remove multiple files
rm file1.txt file2.txt file3.txt

# Interactive removal with confirmation
rm -i important_file.txt

# Force removal without prompting
rm -f read_only_file.txt

# Recursive directory removal
rm -r old_directory/

# Verbose recursive removal
rm -vr directory_to_remove/

# Secure deletion with overwrite
rm -P sensitive_data.txt

# Remove empty directory
rm -d empty_folder/

# Interactive recursive removal
rm -ir directory/

# Don't cross device boundaries
rm -rx /mnt/other_device/directory/

# Verbose force removal
rm -vf *.tmp
```

## Safety Best Practices

### Recommended Usage Patterns
- Use `-i` for important file operations
- Always verify paths before using wildcards
- Use `-I` for bulk operations as a safety compromise
- Test with `ls` before using `rm` with patterns
- Consider using `rm -i` as a system alias

### Dangerous Patterns to Avoid
- `rm -rf /` (attempts to remove entire file system)
- `rm -rf *` in system directories
- Using `rm` with unchecked variables in scripts
- Recursive removal without verification
- Force removal without understanding consequences

## Standards Compliance

The MINIX rm implementation follows:
- IEEE Std 1003.1 (POSIX.1)
- Single UNIX Specification (SUS)
- NetBSD compatibility standards
- BSD historical behavior for compatibility

## Error Handling and Exit Codes

### Exit Status
- **0**: All specified files removed successfully
- **1**: One or more files could not be removed
- **>1**: Serious error occurred during operation

### Common Error Conditions
- Permission denied
- File not found
- Directory not empty (without -r)
- Device busy or mounted
- Symbolic link loops
- Signal interruption

## Security Considerations

### Permission Validation
- Validates file permissions before removal
- Respects MINIX security model
- Proper handling of setuid/setgid files
- Directory permission checking

### Safe Defaults
- Interactive confirmation for dangerous operations
- Non-recursive by default (requires explicit -r)
- Clear warnings for bulk operations
- Comprehensive error checking

### Secure Deletion
- Optional secure overwrite with -P flag
- Multiple overwrite passes
- Regular file restriction
- Prevents simple data recovery

This implementation provides a comprehensive, safety-focused rm utility that balances powerful file removal capabilities with extensive protection mechanisms. The multiple safety layers, comprehensive error handling, and standards compliance make it suitable for both casual use and system administration across all MINIX architectures, including RISC-V 64-bit.