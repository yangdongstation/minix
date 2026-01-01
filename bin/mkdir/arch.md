# MINIX mkdir - Directory Creation Utility

## Overview

The `mkdir` utility is a fundamental file system manipulation tool that creates directories in MINIX 3. As a core UNIX command, it provides essential directory creation capabilities with support for parent directory creation, custom permissions, and verbose operation reporting. The utility serves as the primary interface for creating directory structures in the MINIX file system.

## Purpose and Functionality

mkdir performs directory creation operations including:
1. **Single Directory Creation**: Create individual directories with specified permissions
2. **Parent Directory Creation**: Automatically create missing parent directories
3. **Permission Control**: Set custom permissions during creation
4. **Multiple Directory Creation**: Create multiple directories in a single operation
5. **Verbose Operation**: Report creation operations as they occur

The utility implements intelligent error handling and provides clear feedback about creation success or failure, making it suitable for both interactive use and shell scripting.

## Directory Structure

```
bin/mkdir/
├── mkdir.c        # Main implementation source
├── mkdir.1        # Manual page documentation
├── Makefile       # Build configuration
└── obj/           # Build output directory
```

## Source Files

### mkdir.c - Main Implementation
The complete implementation of the mkdir utility:

- **Lines**: ~150 lines of C code
- **Architecture**: NetBSD-derived implementation
- **License**: BSD 3-clause (University of California/Berkeley)

**Key Functions:**
- `main()`: Command-line parsing and overall program control
- `usage()`: Help text and error message display
- Directory creation logic with error handling
- Permission setting and validation
- Multiple directory processing

**Command-Line Options:**
- `-m mode`: Set file mode (permissions) for created directories
- `-p`: Create parent directories as needed
- `-v`: Verbose mode, print a message for each created directory

### mkdir.1 - Manual Page
Comprehensive documentation including:
- Complete option descriptions
- Permission mode specifications
- Examples of common usage patterns
- Exit status codes
- Standards compliance information

## Build Configuration

### Makefile
```makefile
PROG=    mkdir
.include <bsd.prog.mk>
```

**Build Process:**
- Standard MINIX BSD make infrastructure
- Links against standard C library (libc)
- No special dependencies or compilation flags
- Cross-platform compilation support for all MINIX architectures

## Implementation Details

### Directory Creation Logic
The mkdir utility implements a straightforward directory creation algorithm:

1. **Permission Processing**: Parse and validate permission modes from `-m` option
2. **Directory Creation**: Use `mkdir()` system call to create directory
3. **Permission Setting**: Apply specified permissions if different from defaults
4. **Error Handling**: Provide detailed error messages for failure conditions
5. **Verbose Reporting**: Display success messages when `-v` option is used

### Parent Directory Creation
The `-p` option enables sophisticated parent directory creation:
- **Path Parsing**: Break down target path into components
- **Incremental Creation**: Create missing parent directories step by step
- **Existing Directory Handling**: Skip directories that already exist
- **Error Recovery**: Proper cleanup on partial failure
- **Permission Inheritance**: Apply appropriate permissions to parent directories

### Permission Handling
Flexible permission control mechanisms:
- **Default Permissions**: Use system default (umask-based) when no `-m` specified
- **Symbolic Modes**: Support symbolic permission notation (e.g., 755, a+rwx)
- **Octal Modes**: Support octal permission notation
- **Permission Validation**: Validate and sanitize permission values
- **umask Application**: Proper application of file mode creation mask

### Error Handling
Comprehensive error detection and reporting:
- **Permission Denied**: Handle insufficient privileges gracefully
- **File Exists**: Distinguish between existing files and directories
- **Invalid Path**: Validate path components and syntax
- **System Resource**: Handle system limits and resource exhaustion
- **Parent Directory**: Ensure parent directory exists (without -p)

## MINIX Integration

### System Dependencies
- MINIX VFS (Virtual File System) for directory operations
- Standard C library file system functions
- MINIX permission and security model
- Process permission validation

### Service Interaction
- Uses VFS for all directory creation operations
- Integrates with MINIX permission system
- Respects MINIX file system semantics
- Handles MINIX-specific permission requirements

## RISC-V 64-bit Considerations

### Architecture-Specific Notes
- No RISC-V specific code required (portable implementation)
- Efficient system call interface on RISC-V architecture
- Large file system support with 64-bit operations
- Parent directory creation scales well with RISC-V performance

### Performance Characteristics
- Directory creation operations efficient on RISC-V
- Permission setting optimized for RISC-V system calls
- Parent directory traversal benefits from RISC-V memory architecture
- No alignment or memory model issues on RISC-V

### Cross-Compilation
- Builds successfully with riscv64-unknown-elf-gcc
- No architecture-specific compilation flags needed
- Standard NetBSD portability ensures RISC-V compatibility

## Advanced Features

### Verbose Operation
The `-v` option provides detailed operation reporting:
- **Creation Reporting**: Display each directory as it is created
- **Parent Directory Messages**: Show parent directory creation when using -p
- **Existing Directory Handling**: Report when directories already exist
- **Error Context**: Provide context for error conditions

### Permission Mode Flexibility
Sophisticated permission handling:
- **Symbolic Notation**: Support for ugoa+-=rwxXst notation
- **Octal Notation**: Traditional octal permission specification
- **Combination Modes**: Multiple permission changes in single mode string
- **Special Bits**: Support for setuid, setgid, and sticky bits

### Multiple Directory Creation
Efficient handling of multiple directory arguments:
- **Sequential Processing**: Create directories in argument order
- **Error Isolation**: Continue processing after individual failures
- **Batch Permission Application**: Apply same permissions to all directories
- **Exit Status Aggregation**: Report overall success/failure status

### Path Component Validation
Robust path handling and validation:
- **Component Validation**: Validate each path component
- **Length Checking**: Ensure path lengths are within system limits
- **Character Validation**: Check for invalid characters in path names
- **Relative vs. Absolute**: Handle both relative and absolute path specifications

## Testing and Validation

### Test Coverage
- Basic directory creation functionality
- Parent directory creation with -p option
- Permission mode specification and validation
- Verbose operation reporting
- Multiple directory creation
- Error condition handling
- Existing directory handling

### Integration Tests
- Part of MINIX base system test suite
- Cross-architecture compatibility testing
- Permission system integration validation
- Standards compliance verification

## Usage Examples

```bash
# Create a single directory
mkdir new_directory

# Create directory with specific permissions
mkdir -m 755 new_directory

# Create directory with restricted permissions
mkdir -m 700 private_directory

# Create parent directories as needed
mkdir -p path/to/new/directory/

# Verbose creation with parent directories
mkdir -vp path/to/another/directory/

# Create multiple directories
mkdir dir1 dir2 dir3

# Create multiple directories with permissions
mkdir -m 755 dir1 dir2 dir3

# Complex path with permissions and verbosity
mkdir -vp -m 750 /very/long/path/to/new/directory/

# Create directory with symbolic permissions
mkdir -m u+rwx,g+rx,o-rwx secure_directory
```

## Standards Compliance

The MINIX mkdir implementation follows:
- IEEE Std 1003.1 (POSIX.1)
- Single UNIX Specification (SUS)
- NetBSD compatibility standards
- BSD historical behavior for compatibility

## Error Handling and Exit Codes

### Exit Status
- **0**: All directories created successfully
- **1**: One or more directories could not be created
- **>1**: Serious error occurred during operation

### Common Error Conditions
- Permission denied
- File exists (not a directory)
- Parent directory does not exist (without -p)
- Invalid permission mode
- Path component too long
- System resource limits exceeded

## Security Considerations

### Permission Validation
- Validates directory permissions against system policy
- Respects MINIX security model
- Proper handling of umask application
- Directory permission inheritance

### Path Validation
- Validates path components for security
- Prevents directory traversal attacks
- Handles symbolic links appropriately
- Ensures proper access control

### Safe Defaults
- Reasonable default permissions
- Clear error messages for security violations
- Proper handling of privileged operations
- Integration with MINIX security framework

This implementation provides a robust, user-friendly mkdir utility that handles the essential task of directory creation while providing advanced features for complex directory structure creation. The comprehensive error handling, flexible permission control, and standards compliance make it suitable for both interactive use and shell scripting across all MINIX architectures, including RISC-V 64-bit.