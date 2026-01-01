# MINIX rmdir - Directory Removal Utility

## Overview

The `rmdir` utility is a specialized directory removal tool that provides safe and controlled deletion of empty directories in MINIX 3. Unlike the more powerful `rm` command, rmdir is designed specifically for removing empty directories, providing an additional layer of safety against accidental data loss. It serves as the preferred method for directory cleanup when the emptiness of directories needs to be verified before removal.

## Purpose and Functionality

rmdir performs controlled directory removal operations including:
1. **Empty Directory Removal**: Remove directories only when they are empty
2. **Parent Directory Cleanup**: Remove empty parent directories recursively
3. **Multiple Directory Removal**: Remove multiple empty directories in a single operation
4. **Verbose Operation**: Report removal operations as they occur
5. **Safety Validation**: Ensure directories are empty before removal

The utility implements strict validation to ensure directories are truly empty before removal, making it safer than using `rm -r` for directory cleanup tasks.

## Directory Structure

```
bin/rmdir/
├── rmdir.c        # Main implementation source
├── rmdir.1        # Manual page documentation
├── Makefile       # Build configuration
└── obj/           # Build output directory
```

## Source Files

### rmdir.c - Main Implementation
The complete implementation of the rmdir utility:

- **Lines**: ~100 lines of C code
- **Architecture**: NetBSD-derived implementation
- **License**: BSD 3-clause (University of California/Berkeley)

**Key Functions:**
- `main()`: Command-line parsing and overall program control
- `usage()`: Help text and error message display
- Directory removal logic with empty validation
- Parent directory processing for `-p` option
- Multiple directory argument handling

**Command-Line Options:**
- `-p`: Remove empty parent directories recursively
- `-v`: Verbose mode, print a message for each removed directory

### rmdir.1 - Manual Page
Comprehensive documentation including:
- Complete option descriptions with safety implications
- Examples of safe usage patterns
- Exit status codes and error conditions
- Comparison with rm command
- Standards compliance information

## Build Configuration

### Makefile
```makefile
PROG=    rmdir
.include <bsd.prog.mk>
```

**Build Process:**
- Standard MINIX BSD make infrastructure
- Links against standard C library (libc)
- No special dependencies or compilation flags
- Cross-platform compilation support for all MINIX architectures

## Implementation Details

### Empty Directory Validation
The rmdir utility implements strict validation to ensure directories are empty:

1. **Directory Opening**: Attempt to open directory using `opendir()`
2. **Content Scanning**: Read directory entries using `readdir()`
3. **Entry Filtering**: Skip `.` and `..` entries
4. **Content Detection**: Fail if any other entries are found
5. **Removal Operation**: Use `rmdir()` system call for actual removal

### Parent Directory Removal
The `-p` option enables recursive removal of empty parent directories:
- **Path Analysis**: Break down directory path into components
- **Bottom-Up Processing**: Remove directories from deepest to shallowest
- **Empty Validation**: Verify each parent directory is empty
- **Incremental Removal**: Remove one directory at a time
- **Error Handling**: Stop on first non-empty directory encountered

### Error Handling
Comprehensive error detection and reporting:
- **Permission Denied**: Handle insufficient privileges gracefully
- **Directory Not Empty**: Provide clear error message
- **Invalid Path**: Validate path existence and syntax
- **System Resource**: Handle system limits and resource exhaustion
- **Parent Directory**: Ensure proper error context for `-p` operations

## MINIX Integration

### System Dependencies
- MINIX VFS (Virtual File System) for directory operations
- Standard C library directory functions (opendir, readdir, closedir)
- MINIX permission and security model
- Process permission validation

### Service Interaction
- Uses VFS for all directory removal operations
- Integrates with MINIX permission system
- Respects MINIX file system semantics
- Handles MINIX-specific directory requirements

## RISC-V 64-bit Considerations

### Architecture-Specific Notes
- No RISC-V specific code required (portable implementation)
- Efficient directory reading on RISC-V architecture
- Large directory support with 64-bit file system operations
- Parent directory removal scales well with RISC-V performance

### Performance Characteristics
- Directory entry scanning efficient on RISC-V
- System call overhead minimal on RISC-V
- Parent directory traversal benefits from RISC-V memory architecture
- No alignment or memory model issues on RISC-V

### Cross-Compilation
- Builds successfully with riscv64-unknown-elf-gcc
- No architecture-specific compilation flags needed
- Standard NetBSD portability ensures RISC-V compatibility

## Advanced Features

### Verbose Operation
The `-v` option provides detailed operation reporting:
- **Removal Reporting**: Display each directory as it is removed
- **Parent Directory Messages**: Show parent directory removal with -p
- **Error Context**: Provide context for error conditions
- **Operation Confirmation**: Give visual feedback for successful operations

### Multiple Directory Processing
Efficient handling of multiple directory arguments:
- **Sequential Processing**: Remove directories in argument order
- **Error Isolation**: Continue processing after individual failures
- **Batch Reporting**: Aggregate results for multiple operations
- **Exit Status Aggregation**: Report overall success/failure status

### Path Component Analysis
Robust path handling and validation:
- **Component Validation**: Validate each path component
- **Relative vs. Absolute**: Handle both relative and absolute paths
- **Symbolic Link Handling**: Proper handling of symbolic links in paths
- **Path Length Validation**: Ensure paths are within system limits

### Directory Entry Filtering
Sophisticated directory content analysis:
- **Dot Entry Filtering**: Skip `.` and `..` entries
- **Hidden File Detection**: Detect files starting with dot
- **Subdirectory Detection**: Identify nested directories
- **Special File Detection**: Handle special files appropriately

## Testing and Validation

### Test Coverage
- Basic empty directory removal
- Parent directory removal with -p option
- Verbose operation reporting
- Multiple directory removal
- Error condition handling
- Non-empty directory rejection
- Permission validation

### Integration Tests
- Part of MINIX base system test suite
- Cross-architecture compatibility testing
- Directory entry scanning validation
- Standards compliance verification

## Usage Examples

```bash
# Remove a single empty directory
rmdir empty_directory

# Remove multiple empty directories
rmdir dir1 dir2 dir3

# Verbose removal of empty directory
rmdir -v empty_directory

# Remove empty directory and empty parents
rmdir -p path/to/empty/directory/

# Verbose recursive parent removal
rmdir -vp path/to/empty/directory/

# Remove multiple directories with parent cleanup
rmdir -p dir1/subdir/ dir2/subdir/

# Attempt to remove non-empty directory (will fail)
rmdir non_empty_directory

# Remove deeply nested empty directory structure
rmdir -vp very/deep/nested/empty/directory/
```

## Standards Compliance

The MINIX rmdir implementation follows:
- IEEE Std 1003.1 (POSIX.1)
- Single UNIX Specification (SUS)
- NetBSD compatibility standards
- BSD historical behavior for compatibility

## Error Handling and Exit Codes

### Exit Status
- **0**: All directories removed successfully
- **1**: One or more directories could not be removed
- **>1**: Serious error occurred during operation

### Common Error Conditions
- Directory not empty
- Permission denied
- Directory does not exist
- Invalid path specification
- System resource limits
- Parent directory not empty (with -p)

## Comparison with rm

### Safety Advantages
- **Empty Directory Only**: Prevents removal of non-empty directories
- **No Recursive Default**: Safer than rm -r for directory cleanup
- **Explicit Validation**: Forces verification of directory emptiness
- **Limited Scope**: Cannot accidentally remove files

### Functional Limitations
- **Empty Directories Only**: Cannot remove directories with contents
- **No File Removal**: Cannot remove individual files
- **No Force Option**: Always validates directory emptiness
- **Limited Recursive**: Only removes empty parent directories

### Use Case Recommendations
- Use rmdir for safe directory cleanup
- Use rm -r only when directory contents should be removed
- Use rmdir -p for hierarchical empty directory removal
- Prefer rmdir in scripts for safety

## Security Considerations

### Permission Validation
- Validates directory permissions before removal
- Respects MINIX security model
- Proper handling of directory ownership
- Parent directory permission checking

### Path Validation
- Validates path components for security
- Prevents directory traversal attacks
- Handles symbolic links appropriately
- Ensures proper access control

### Safe Defaults
- Empty directory validation
- Clear error messages for security violations
- Proper handling of privileged operations
- Integration with MINIX security framework

This implementation provides a safe, reliable rmdir utility that serves as the preferred method for removing empty directories in MINIX 3. The strict validation requirements, comprehensive error handling, and safety-focused design make it an essential tool for directory management across all MINIX architectures, including RISC-V 64-bit.