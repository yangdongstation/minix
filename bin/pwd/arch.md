# MINIX pwd - Print Working Directory Utility

## Overview

The `pwd` utility is a fundamental file system navigation tool that displays the current working directory path in MINIX 3. As one of the most basic yet essential UNIX commands, it provides users and scripts with the ability to determine their current location within the file system hierarchy. The utility implements both logical and physical path resolution strategies to handle symbolic links and provide accurate directory information.

## Purpose and Functionality

pwd performs current directory path determination including:
1. **Logical Path Display**: Show path containing symbolic links (default)
2. **Physical Path Display**: Show actual file system path without symbolic links
3. **Path Validation**: Verify current directory accessibility and validity
4. **Error Reporting**: Handle situations where current directory is inaccessible
5. **Script Integration**: Provide reliable path information for shell scripts

The utility serves as a critical component for file system navigation, path resolution, and system administration tasks where knowing the exact current location is essential.

## Directory Structure

```
bin/pwd/
├── pwd.c          # Main implementation source
├── pwd.1          # Manual page documentation
├── Makefile       # Build configuration
└── obj/           # Build output directory
```

## Source Files

### pwd.c - Main Implementation
The complete implementation of the pwd utility:

- **Lines**: ~150 lines of C code
- **Architecture**: NetBSD-derived implementation
- **License**: BSD 3-clause (University of California/Berkeley)

**Key Functions:**
- `main()`: Command-line parsing and operation selection
- `usage()`: Help text and error message display
- Path resolution using getcwd() system call
- Logical vs. physical path determination
- Error handling for inaccessible directories

**Command-Line Options:**
- `-L`: Display logical path (default, contains symbolic links)
- `-P`: Display physical path (resolves all symbolic links)
- `-h`: Display help message and exit

### pwd.1 - Manual Page
Comprehensive documentation including:
- Complete option descriptions
- Logical vs. physical path explanations
- Examples of common usage patterns
- Exit status codes and error conditions
- Standards compliance information

## Build Configuration

### Makefile
```makefile
PROG=    pwd
.include <bsd.prog.mk>
```

**Build Process:**
- Standard MINIX BSD make infrastructure
- Links against standard C library (libc)
- No special dependencies or compilation flags
- Cross-platform compilation support for all MINIX architectures

## Implementation Details

### Path Resolution Strategies
The pwd utility implements two distinct path resolution approaches:

**Logical Path Resolution (-L):**
- **Default Behavior**: Shows path as user navigated (with symbolic links)
- **Symbolic Link Preservation**: Maintains symbolic links in path
- **User Expectation**: Matches user's navigation experience
- **Shell Integration**: Consistent with PWD environment variable

**Physical Path Resolution (-P):**
- **Symbolic Link Resolution**: Resolves all symbolic links to actual directories
- **Actual File System Path**: Shows true file system location
- **Canonical Path**: Provides shortest, unambiguous path
- **System Consistency**: Same path regardless of navigation method

### System Call Integration
Sophisticated use of system calls for path determination:

**getcwd() System Call:**
- **Primary Method**: Main system call for current directory retrieval
- **Buffer Management**: Dynamic buffer allocation for path storage
- **Error Handling**: Comprehensive error detection and reporting
- **Memory Efficiency**: Optimal memory usage for path storage

**PATH_MAX Considerations:**
- **System Limits**: Respect system path length limitations
- **Dynamic Allocation**: Allocate appropriate buffer sizes
- **Path Validation**: Verify path lengths are within limits
- **Error Recovery**: Handle overly long paths gracefully

### Error Handling
Comprehensive error detection and reporting:
- **Permission Denied**: Handle insufficient directory access permissions
- **Directory Removal**: Handle cases where current directory was deleted
- **File System Issues**: Handle file system corruption or unmounting
- **Memory Allocation**: Handle memory allocation failures for path storage
- **System Limits**: Handle system resource limitations

## MINIX Integration

### System Dependencies
- MINIX VFS (Virtual File System) for directory operations
- Standard C library file system functions (getcwd())
- MINIX process and directory management
- MINIX file system permission model

### Service Interaction
- Uses VFS for current directory path resolution
- Integrates with MINIX process management
- Respects MINIX file system semantics
- Handles MINIX-specific directory requirements

## RISC-V 64-bit Considerations

### Architecture-Specific Notes
- No RISC-V specific code required (portable implementation)
- Efficient system call interface on RISC-V architecture
- Large path support with 64-bit memory addressing
- Path resolution scales well with RISC-V performance

### Performance Characteristics
- getcwd() system call efficient on RISC-V
- Memory allocation optimized for RISC-V
- Path manipulation benefits from RISC-V instruction set
- No alignment or memory model issues on RISC-V

### Cross-Compilation
- Builds successfully with riscv64-unknown-elf-gcc
- No architecture-specific compilation flags needed
- Standard NetBSD portability ensures RISC-V compatibility

## Advanced Features

### Logical vs. Physical Path Selection
Sophisticated path display options:
- **User Experience**: Logical path matches user navigation
- **System Accuracy**: Physical path shows actual file system location
- **Script Compatibility**: Both modes suitable for different use cases
- **Default Behavior**: Logical path maintains user expectations

### Error Recovery
Robust error handling for edge cases:
- **Deleted Directory**: Handle current directory deletion gracefully
- **Permission Changes**: Handle permission revocation during operation
- **File System Unmounting**: Handle file system unmounting gracefully
- **Network Issues**: Handle network file system disconnection

### Script Integration
Optimized for shell script usage:
- **Consistent Output**: Reliable path format for parsing
- **Exit Codes**: Clear success/failure indication
- **Error Messages**: Standard error output for error handling
- **Path Validation**: Ensure returned paths are valid and accessible

## Path Resolution Examples

### Basic Usage Examples
```bash
# Display current logical directory
pwd

# Display current physical directory (resolve symlinks)
pwd -P

# Explicit logical directory (same as default)
pwd -L

# Display help information
pwd -h
```

### Symbolic Link Scenarios
```bash
# Navigate through symbolic link
cd /home/user/symlink_to_directory

# Show logical path (contains symlink)
pwd
# Output: /home/user/symlink_to_directory

# Show physical path (resolves symlink)
pwd -P
# Output: /actual/path/to/directory
```

### Script Integration Examples
```bash
# Store current directory in variable
CURRENT_DIR=$(pwd)

# Use in conditional statements
if [ "$(pwd)" = "/home/user" ]; then
    echo "In home directory"
fi

# Combine with other commands
cd $(pwd)/subdir

# Use physical path for system operations
PHYSICAL_PATH=$(pwd -P)
```

## Testing and Validation

### Test Coverage
- Basic path display functionality
- Logical vs. physical path resolution
- Symbolic link handling
- Error condition responses
- Script integration scenarios
- Permission and access validation

### Integration Tests
- Part of MINIX base system test suite
- Cross-architecture compatibility testing
- getcwd() system call validation
- Standards compliance verification
- Shell integration testing

## Error Handling and Exit Codes

### Exit Status
- **0**: Current directory path successfully determined
- **1**: Current directory path could not be determined
- **>1**: Serious error occurred during operation

### Common Error Conditions
- Current directory has been removed
- Insufficient permissions to read directory
- File system corruption or unmounting
- Memory allocation failure
- System resource limits exceeded
- Symbolic link loops in path resolution

## Comparison with Shell Built-ins

### pwd Utility vs. Shell Built-in
**Advantages of External pwd:**
- **Consistency**: Same behavior across different shells
- **Standards Compliance**: Follows POSIX standards strictly
- **Reliability**: Independent of shell implementation
- **Error Handling**: Comprehensive error reporting

**Shell Built-in Considerations:**
- **Performance**: May be faster as built-in
- **Environment**: May use PWD environment variable
- **Implementation**: Behavior may vary between shells
- **Features**: May have additional shell-specific features

### Use Case Recommendations
- Use external pwd for scripts requiring portability
- Use external pwd for consistent error handling
- Use external pwd for system administration tasks
- Consider built-in for interactive shell usage

## Security Considerations

### Path Validation
- Validates current directory accessibility
- Ensures returned paths are safe for use
- Handles symbolic links securely
- Prevents path injection attacks

### Permission Handling
- Validates directory read permissions
- Handles permission changes gracefully
- Respects MINIX security model
- Provides clear error messages for security violations

### Safe Defaults
- Reliable path resolution
- Clear error reporting
- Proper handling of edge cases
- Integration with MINIX security framework

## Performance Considerations

### Path Resolution Efficiency
- Efficient getcwd() system call usage
- Minimal memory allocation overhead
- Optimal buffer size selection
- Fast error detection and reporting

### System Resource Usage
- Minimal CPU usage for path resolution
- Efficient memory usage patterns
- No file system traversal required
- Optimized for frequent usage

### Scalability
- Handles deep directory hierarchies
- Supports long path names
- Efficient with large directory trees
- No performance degradation with system size

This implementation provides a reliable, standards-compliant pwd utility that serves as the fundamental tool for current directory determination in MINIX 3. The dual path resolution modes, comprehensive error handling, and script-friendly output make it essential for both interactive use and system administration tasks across all MINIX architectures, including RISC-V 64-bit.