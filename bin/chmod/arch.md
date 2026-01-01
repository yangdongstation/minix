# MINIX chmod - File Permission Utility

## Overview

The `chmod` utility is a fundamental file system security tool that modifies file and directory permissions in MINIX 3. It provides comprehensive permission management capabilities, supporting both symbolic and octal notation modes, recursive directory operations, and various reference-based permission settings. The utility serves as the primary interface for controlling file system access rights in the MINIX security model.

## Purpose and Functionality

chmod performs file permission modification operations including:
1. **Permission Setting**: Set absolute permissions using octal or symbolic modes
2. **Permission Modification**: Add or remove specific permissions
3. **Recursive Operations**: Apply permissions to directory trees
4. **Reference-Based Permissions**: Copy permissions from reference files
5. **Selective Permission Changes**: Target specific user categories (user, group, others)

The utility implements sophisticated permission parsing and validation, supporting both traditional UNIX permission models and extended MINIX security features.

## Directory Structure

```
bin/chmod/
├── chmod.c        # Main implementation source
├── chmod.1        # Manual page documentation
├── Makefile       # Build configuration
└── obj/           # Build output directory
```

## Source Files

### chmod.c - Main Implementation
The comprehensive implementation of the chmod utility:

- **Lines**: ~200 lines of C code
- **Architecture**: NetBSD-derived implementation
- **License**: BSD 3-clause (University of California/Berkeley)

**Key Functions:**
- `main()`: Command-line parsing and overall program control
- `usage()`: Help text and error message display
- Permission mode parsing and validation
- File permission application with error handling
- Recursive directory processing
- Reference file permission copying

**Command-Line Options:**
- `-R`: Change permissions recursively through directory trees
- `-H`: Follow symbolic links specified on command line (with -R)
- `-L`: Follow all symbolic links (with -R)
- `-P`: Never follow symbolic links (default with -R)
- `-f`: Suppress most error messages
- `-h`: Don't treat symbolic links specially
- `-v`: Verbose mode, show permissions as they are changed

### chmod.1 - Manual Page
Comprehensive documentation including:
- Complete option descriptions
- Permission mode specifications and examples
- Symbolic and octal notation explanations
- Recursive operation behavior
- Security considerations and best practices
- Standards compliance information

## Build Configuration

### Makefile
```makefile
PROG=    chmod
.include <bsd.prog.mk>
```

**Build Process:**
- Standard MINIX BSD make infrastructure
- Links against standard C library (libc)
- No special dependencies or compilation flags
- Cross-platform compilation support for all MINIX architectures

## Implementation Details

### Permission Mode Parsing
The chmod utility implements sophisticated permission mode parsing:

**Octal Notation:**
- **Format**: Three or four octal digits (e.g., 755, 0644)
- **Position Mapping**: User (owner), group, others
- **Permission Bits**: Read (4), write (2), execute (1)
- **Special Bits**: Setuid (4), setgid (2), sticky (1)

**Symbolic Notation:**
- **Format**: `[ugoa...][[+-=][rwxXst...]...][,...]`
- **User Categories**: u (user), g (group), o (others), a (all)
- **Operations**: + (add), - (remove), = (set exactly)
- **Permissions**: r (read), w (write), x (execute), X (conditional execute)
- **Special Bits**: s (setuid/setgid), t (sticky bit)

### Permission Application Logic
Intelligent permission application based on file types:

**Regular Files:**
- Apply all specified permissions
- Handle setuid/setgid bits appropriately
- Validate permission combinations

**Directories:**
- Apply permissions with directory-specific semantics
- Handle execute permission as directory access
- Manage setgid bit for group inheritance

**Symbolic Links:**
- Default behavior: modify link permissions (usually ineffective)
- `-h` option: modify link permissions explicitly
- Follow link options: modify target file permissions

**Special Files:**
- Device file permission handling
- Named pipe permission management
- Socket file permission control

### Recursive Directory Processing
Sophisticated recursive permission changes:
- **FTS Library Usage**: Efficient directory tree traversal
- **Symbolic Link Handling**: Various follow strategies
- **Error Handling**: Continue processing on individual file failures
- **Permission Inheritance**: Maintain directory traversal permissions

## MINIX Integration

### System Dependencies
- MINIX VFS (Virtual File System) for permission operations
- Standard C library file system functions
- MINIX permission and security model
- FTS library for directory tree traversal

### Service Interaction
- Uses VFS for all permission modification operations
- Integrates with MINIX permission system
- Respects MINIX file system semantics
- Handles MINIX-specific security features

## RISC-V 64-bit Considerations

### Architecture-Specific Notes
- No RISC-V specific code required (portable implementation)
- Efficient permission bit manipulation on RISC-V
- Large file system support with 64-bit operations
- Recursive operations scale well with RISC-V performance

### Performance Characteristics
- Permission setting operations efficient on RISC-V
- Directory tree traversal benefits from RISC-V memory architecture
- Symbolic link processing optimized for RISC-V
- No alignment or memory model issues on RISC-V

### Cross-Compilation
- Builds successfully with riscv64-unknown-elf-gcc
- No architecture-specific compilation flags needed
- Standard NetBSD portability ensures RISC-V compatibility

## Advanced Features

### Reference File Permissions
Copy permissions from existing files:
- **--reference=file**: Copy permissions from reference file
- **Permission Inheritance**: Inherit exact permission bits
- **Cross-File-Type**: Handle different file type conversions
- **Error Handling**: Validate reference file accessibility

### Verbose Operation
The `-v` option provides detailed operation reporting:
- **Permission Display**: Show old and new permissions
- **File Identification**: Identify files being modified
- **Operation Context**: Provide context for permission changes
- **Error Reporting**: Enhanced error message detail

### Symbolic Link Strategies
Multiple approaches for symbolic link handling:
- **Default Behavior**: Modify link permissions (usually no effect)
- **Follow Links**: Modify target file permissions
- **Preserve Links**: Treat links as regular files
- **Mixed Strategies**: Different behavior for command-line vs. recursive

### Error Suppression
The `-f` option enables quiet operation:
- **Permission Errors**: Suppress permission-related error messages
- **File Access**: Handle inaccessible files gracefully
- **Recursive Processing**: Continue despite individual file errors
- **Exit Status**: Maintain accurate exit codes despite suppression

## Permission Mode Examples

### Octal Mode Examples
```bash
# Set read/write/execute for owner, read/execute for group and others
chmod 755 file.txt

# Set read/write for owner, read for group and others
chmod 644 document.txt

# Set read/write/execute for owner, no permissions for others
chmod 700 private_file

# Set setuid bit on executable
chmod 4755 program

# Set sticky bit on directory
chmod 1777 shared_directory
```

### Symbolic Mode Examples
```bash
# Add execute permission for user
chmod u+x script.sh

# Remove write permission for group and others
chmod go-w file.txt

# Set exact permissions for all users
chmod a=r file.txt

# Add read permission for everyone
chmod a+r document.txt

# Remove execute permission for others
chmod o-x program

# Add setgid bit to directory
chmod g+s directory/

# Set conditional execute permission
chmod u+X script.sh
```

### Recursive Operation Examples
```bash
# Recursively set permissions on directory tree
chmod -R 755 project_directory/

# Recursively add execute permission for user
chmod -R u+X script_directory/

# Recursively remove write permission for others
chmod -R o-w shared_directory/

# Verbose recursive permission change
chmod -vR 644 document_directory/

# Follow symbolic links during recursive operation
chmod -RL 755 directory_with_links/
```

### Reference File Examples
```bash
# Copy permissions from reference file
chmod --reference=template.txt target.txt

# Recursive reference-based permission setting
chmod -R --reference=template_dir/ target_dir/

# Verbose reference permission copying
chmod -v --reference=source_file target_file
```

## Testing and Validation

### Test Coverage
- Basic permission setting with octal modes
- Symbolic permission mode parsing and application
- Recursive directory permission changes
- Reference file permission copying
- Symbolic link permission handling
- Error condition handling
- Special permission bit handling

### Integration Tests
- Part of MINIX base system test suite
- Cross-architecture compatibility testing
- Permission system integration validation
- Standards compliance verification

## Security Considerations

### Permission Validation
- Validates permission modes against system policy
- Respects MINIX security model
- Proper handling of setuid/setgid bits
- Directory permission inheritance

### Safe Defaults
- Reasonable permission validation
- Clear error messages for security violations
- Proper handling of privileged operations
- Integration with MINIX security framework

### Permission Inheritance
- Proper handling of permission inheritance
- Directory permission propagation
- Group ownership and permission interaction
- Sticky bit behavior for shared directories

This implementation provides a comprehensive, standards-compliant chmod utility that serves as the primary interface for file permission management in MINIX 3. The sophisticated permission parsing, flexible operation modes, and comprehensive error handling make it suitable for both simple permission changes and complex permission management tasks across all MINIX architectures, including RISC-V 64-bit.