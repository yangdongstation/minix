# MINIX 3 Sed Stream Editor Architecture

## Overview

The `/usr.bin/sed` directory contains the BSD sed (stream editor) utility, a powerful text processing tool that performs non-interactive text transformations. This implementation provides comprehensive stream editing capabilities including pattern matching, text substitution, line addressing, and script execution, making it essential for text processing pipelines and automated editing tasks in the MINIX environment.

## Purpose and Functionality

### Core Stream Editing
- **Non-interactive Processing**: Processes text streams without user interaction
- **Line-Oriented**: Operates on text line by line with pattern matching
- **Script Execution**: Executes sed scripts containing editing commands
- **Regular Expressions**: Full regular expression support for pattern matching
- **Text Transformation**: Substitution, deletion, insertion, and transformation operations

### Advanced Features
- **Addressing**: Line numbers, patterns, and ranges for selective editing
- **Hold Space**: Advanced text manipulation with pattern space and hold space
- **Branching**: Conditional execution and control flow
- **Multi-line Processing**: Operations across line boundaries
- **In-place Editing**: Direct file modification capabilities
- **Extended Regular Expressions**: Enhanced regex syntax support

## Source Code Architecture

### Core Components

#### `main.c` - Program Entry and Initialization
- **Purpose**: Command-line processing and program initialization
- **Key Functions**:
  - `main()`: Entry point, processes command-line arguments
  - `usage()`: Displays usage information and available options
  - `add_compunit()`: Adds compilation units for script processing
  - `process()`: Main processing loop for input files
- **Features**: Supports -e (script), -f (script file), -n (suppress output), -i (in-place)

#### `compile.c` - Script Compilation Engine
- **Purpose**: Compiles sed scripts into internal instruction format
- **Key Functions**:
  - `compile_string()`: Compiles string-based sed scripts
  - `compile_file()`: Compiles sed scripts from files
  - `compile_regex()`: Compiles regular expressions
  - `cmp_adr()`: Compares addresses for range matching
  - `address()`: Parses and validates line addresses
- **Features**: Comprehensive script validation, error reporting, and optimization

#### `process.c` - Execution Engine
- **Purpose**: Executes compiled sed scripts against input streams
- **Key Functions**:
  - `process()`: Main execution loop
  - `match_address()`: Matches line addresses and patterns
  - `match_regex()`: Performs regular expression matching
  - `substitute()`: Executes substitution commands
  - `print_line()`: Handles line output and formatting
- **Features**: Efficient pattern matching, memory management, and error handling

#### `misc.c` - Utility Functions
- **Purpose**: Common utility functions and helper routines
- **Key Functions**:
  - `strregerror()`: Regular expression error reporting
  - `realloc_fgetline()`: Dynamic line reading with buffer management
  - `err()`: Consistent error reporting
- **Features**: Memory-safe string handling and error management

#### `defs.h` - Core Definitions
- **Purpose**: Central header with type definitions and constants
- **Key Structures**:
  - `sed_compunit`: Compilation unit structure
  - `sed_cmd`: Command structure for compiled scripts
  - `str_list`: String list management
  - `re_guts`: Regular expression internal structure
- **Features**: Comprehensive type system for sed operations

#### `extern.h` - External Declarations
- **Purpose**: External function and variable declarations
- **Features**: Clean separation of interface and implementation

## Command Architecture

### Sed Command Structure
```c
struct sed_cmd {
    enum cmd_code code;     /* Command type */
    struct addr *a1, *a2;   /* Address range */
    union {
        struct subst {      /* Substitution data */
            regex_t *re;
            char *replacement;
            int flags;
        } subst;
        char *text;         /* Text data for a/i/c commands */
        struct wfile {      /* File operations */
            char *fname;
            FILE *fp;
        } wfile;
        /* Additional command-specific data */
    } u;
};
```

### Address Types
- **Line Numbers**: Absolute line addressing (1, 10, $)
- **Regular Expressions**: Pattern-based addressing (/pattern/)
- **Ranges**: Line ranges (1,10 or /start/,/end/)
- **Relative**: Relative addressing (+1, -2)

### Command Categories

#### Text Output Commands
- **Print (p)**: Prints pattern space
- **Print Line Number (=)**: Prints current line number
- **List (l)**: Prints pattern space in visual format
- **Write (w)**: Writes to file

#### Text Transformation Commands
- **Substitute (s)**: Pattern substitution with regex support
- **Translate (y)**: Character-by-character translation
- **Transform (y)**: Case conversion and character mapping

#### Text Manipulation Commands
- **Append (a)**: Appends text after current line
- **Insert (i)**: Inserts text before current line
- **Change (c)**: Replaces entire line
- **Delete (d)**: Deletes pattern space

#### Advanced Commands
- **Hold (h/H)**: Manipulates hold space
- **Get (g/G)**: Retrieves from hold space
- **Exchange (x)**: Swaps pattern and hold space
- **Branch (b)**: Unconditional branching
- **Test (t)**: Conditional branching
- **Next (n)**: Reads next line into pattern space

## Build Configuration

### Makefile Structure
```makefile
PROG= sed
SRCS= compile.c main.c misc.c process.c
MAN= sed.1
LDADD= -lutil
DPADD= ${LIBUTIL}

.include <bsd.prog.mk>
```

### Build Dependencies
- **libutil**: Utility library for common functions
- **Regular Expression Library**: System regex support
- **Standard C Library**: Core C runtime support

### Compilation Features
- **Optimization**: Configurable optimization levels
- **Debug Support**: Optional debug symbols and assertions
- **POSIX Compliance**: Strict POSIX sed standard compliance
- **Extended Features**: Additional BSD extensions

## Integration with MINIX System

### Message-Based Architecture
Sed integrates with MINIX through standard system interfaces:
- **VFS Integration**: File I/O through virtual file system
- **Process Management**: Standard process creation and management
- **Memory Management**: Coordinates with VM for memory allocation

### Service Dependencies
- **File System Services**: Heavy use of VFS for file operations
- **Regular Expression Support**: System regex library integration
- **Memory Management**: Coordinates with VM for dynamic allocation

### MINIX-Specific Adaptations
- **Reliability**: Enhanced error checking and recovery
- **Resource Management**: Careful resource allocation and cleanup
- **Memory Efficiency**: Optimized for embedded systems
- **Error Handling**: Consistent with MINIX error reporting

## Advanced Features

### Regular Expression Engine
- **POSIX Compliance**: Full POSIX regular expression support
- **Extended Syntax**: Additional regex features and optimizations
- **Performance**: Efficient regex compilation and execution
- **Error Handling**: Comprehensive regex error reporting

### Memory Management
- **Dynamic Allocation**: Efficient memory allocation for large files
- **Buffer Management**: Optimized line buffering and processing
- **Resource Cleanup**: Proper resource deallocation
- **Memory Safety**: Bounds checking and overflow protection

### Script Optimization
- **Command Merging**: Optimizes adjacent compatible commands
- **Dead Code Elimination**: Removes unreachable commands
- **Pattern Compilation**: Pre-compiles regular expressions
- **Address Calculation**: Optimizes address range calculations

### Multi-file Processing
- **File Chaining**: Seamless processing of multiple input files
- **In-place Editing**: Safe file modification with backup support
- **Stream Processing**: Efficient handling of large data streams
- **Error Continuation**: Continues processing despite individual file errors

## RISC-V 64-bit Considerations

### Architecture Optimization
- **Memory Alignment**: Proper alignment for RISC-V memory requirements
- **String Operations**: Optimized string processing for RISC-V
- **Regular Expression Performance**: Efficient regex execution on RISC-V
- **File I/O Optimization**: Streamlined file operations for embedded systems

### Platform-Specific Features
- **Large File Support**: Handles large text files efficiently
- **Memory Constraints**: Optimized for systems with limited memory
- **Performance Tuning**: RISC-V-specific performance optimizations
- **Cross-Platform Compatibility**: Consistent behavior across architectures

### Testing and Validation
- **RISC-V Specific Tests**: Architecture-specific test validation
- **Performance Benchmarking**: Performance measurement on RISC-V
- **Memory Usage Validation**: Memory consumption analysis
- **Compatibility Testing**: Cross-platform compatibility verification

## Usage Examples

### Basic Text Substitution
```bash
# Simple substitution
sed 's/old/new/' file.txt

# Global substitution
sed 's/old/new/g' file.txt

# In-place editing
sed -i 's/old/new/g' file.txt

# Multiple substitutions
sed -e 's/old/new/' -e 's/foo/bar/' file.txt
```

### Pattern-Based Operations
```bash
# Print lines matching pattern
sed -n '/pattern/p' file.txt

# Delete lines matching pattern
sed '/pattern/d' file.txt

# Replace only in matching lines
sed '/pattern/s/old/new/' file.txt

# Work with line ranges
sed '10,20s/old/new/' file.txt
```

### Advanced Text Manipulation
```bash
# Insert text before pattern
sed '/pattern/i\new text' file.txt

# Append text after pattern
sed '/pattern/a\new text' file.txt

# Change entire line
sed '/pattern/c\replacement line' file.txt

# Use hold space for complex operations
sed -e '/pattern/h' -e '$G' file.txt
```

### Script Files
```bash
# Execute script file
sed -f script.sed file.txt

# Multiple script files
sed -f script1.sed -f script2.sed file.txt

# Complex script with multiple commands
sed '
    /^#/d
    s/foo/bar/g
    10,20s/old/new/
    $a\
    End of file
' input.txt
```

### Stream Processing
```bash
# Process output from other commands
ls -la | sed 's/^/    /'

# Chain with other utilities
cat file.txt | grep pattern | sed 's/foo/bar/'

# Transform file contents
cat input.txt | sed 's/[[:space:]]*$//' > output.txt
```

## Development and Debugging

### Debug Features
- **Script Validation**: Comprehensive script syntax checking
- **Error Reporting**: Detailed error messages with line numbers
- **Verbose Mode**: Detailed execution tracing
- **Regular Expression Debugging**: Regex compilation and matching details

### Error Handling
- **Syntax Errors**: Comprehensive script syntax validation
- **Runtime Errors**: Graceful handling of execution errors
- **File Errors**: Proper handling of file I/O errors
- **Memory Errors**: Memory allocation failure handling

### Testing Infrastructure
- **Script Validation**: Extensive script syntax testing
- **Regular Expression Tests**: Comprehensive regex pattern testing
- **Edge Case Testing**: Boundary condition validation
- **Compatibility Testing**: POSIX compliance verification

## Performance Characteristics

### Scalability
- **Large Files**: Efficient processing of multi-gigabyte files
- **Complex Scripts**: Handles scripts with hundreds of commands
- **Memory Efficiency**: Minimal memory footprint
- **Streaming Processing**: Constant memory usage regardless of file size

### Optimization Features
- **Command Compilation**: Pre-compilation of sed scripts
- **Regular Expression Caching**: Cached regex compilation
- **Buffer Management**: Efficient line buffering
- **Stream Processing**: Single-pass processing where possible

### Benchmarking
- **Text Processing Speed**: Optimized for high-throughput text processing
- **Regular Expression Performance**: Efficient regex engine
- **Memory Usage**: Minimal memory allocation and copying
- **I/O Efficiency**: Optimized file I/O operations

## Integration with Text Processing Pipeline

### Common Combinations
```bash
# Extract and transform
grep pattern file.txt | sed 's/foo/bar/'

# Sort and process
sort file.txt | sed 's/^/Line: /'

# Count and format
wc -l file.txt | sed 's/\([0-9]*\).*/File has \1 lines/'

# Multi-stage processing
sed 's/old/new/' file.txt | awk '{print $1}' | sort | uniq
```

### Script Integration
```bash
# Shell script integration
#!/bin/sh
for file in *.txt; do
    sed 's/OLD/NEW/g' "$file" > "${file%.txt}_new.txt"
done

# Makefile integration
transform:
    sed -f transform.sed input.txt > output.txt
```

This sed implementation provides a comprehensive, POSIX-compliant stream editor with advanced features optimized for the MINIX environment and RISC-V 64-bit architecture, forming a crucial component of the text processing toolchain.