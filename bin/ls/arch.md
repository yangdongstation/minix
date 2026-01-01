# MINIX ls - Directory Listing Utility

## Overview

The `ls` utility is a sophisticated directory listing program that provides comprehensive file system information display capabilities. As one of the most frequently used UNIX commands, ls serves as the primary interface for exploring file system contents in MINIX 3, offering multiple output formats and extensive filtering options.

## Purpose and Functionality

ls performs advanced directory listing operations with features including:
1. **Basic File Listing**: Display file and directory names
2. **Detailed File Information**: Show permissions, ownership, size, and timestamps
3. **Recursive Directory Traversal**: List contents of directory trees
4. **Multiple Output Formats**: Various sorting and display options
5. **File Filtering**: Pattern matching and attribute-based filtering
6. **Human-Readable Output**: Size formatting and time display options

## Directory Structure

```
bin/ls/
├── ls.c           # Core implementation and main logic
├── ls.h           # Header file with structures and global declarations
├── main.c         # Entry point wrapper
├── print.c        # Output formatting and display functions
├── util.c         # Utility functions for file operations
├── cmp.c          # File comparison and sorting functions
├── extern.h       # External function declarations
├── ls.1           # Manual page documentation
├── Makefile       # Build configuration
└── obj/           # Build output directory
```

## Source Files

### ls.c - Core Implementation
The main implementation file containing the bulk of ls functionality:

- **Lines**: ~600 lines of C code
- **Architecture**: NetBSD-derived implementation
- **License**: BSD 3-clause (University of California/Berkeley)

**Key Functions:**
- `ls_main()`: Main entry point and overall program flow
- `display()`: Core display logic for different output formats
- `traverse()`: Directory tree traversal using FTS
- `sort()`: Multi-level file sorting implementation
- `printscol()`: Single column output formatting
- `printlong()`: Long format (detailed) output
- `printstream()`: Stream-based output for large directories

**Global Configuration Flags:**
- `f_longform`: Enable long format display (-l)
- `f_inode`: Show inode numbers (-i)
- `f_size`: Show file sizes (-s)
- `f_type`: Add type indicators (-F)
- `f_accesstime`: Use access time (-u)
- `f_statustime`: Use status change time (-c)
- `f_humanize`: Human-readable sizes (-h)
- `f_commas`: Comma-separated sizes
- `f_flags`: Show file flags (-o)

### ls.h - Header Definitions
Comprehensive header file defining data structures and global variables:

**Key Structures:**
```c
typedef struct {
    FTSENT *list;           /* File list from FTS */
    u_int64_t btotal;       /* Total blocks */
    u_int64_t stotal;       /* Total size */
    int entries;            /* Number of entries */
    unsigned int maxlen;    /* Maximum name length */
    int s_block;            /* Block size field width */
    int s_flags;            /* Flags field width */
    int s_group;            /* Group name field width */
    int s_inode;            /* Inode field width */
    int s_nlink;            /* Link count field width */
    int s_size;             /* Size field width */
    int s_user;             /* User name field width */
    int s_major;            /* Major device number width */
    int s_minor;            /* Minor device number width */
} DISPLAY;

typedef struct {
    char *user;             /* User name */
    char *group;            /* Group name */
    char *flags;            /* File flags */
    char data[1];           /* Variable length data */
} NAMES;
```

**Global Variables:**
- `blocksize`: File system block size for block calculations
- Various format flags for different display options
- Column width variables for output formatting

### main.c - Entry Point
Simple wrapper that calls the main ls function:
- Provides clean separation between entry point and implementation
- Enables easy integration with different build systems
- Maintains NetBSD compatibility

### print.c - Output Formatting
Sophisticated output formatting functions for different display modes:

**Key Functions:**
- `printscol()`: Single column formatting
- `printlong()`: Long format with detailed file information
- `printcol()`: Multi-column formatting
- `printstream()`: Stream output for performance
- `printname()`: Individual file name formatting
- `printtype()`: File type indicator addition

**Advanced Features:**
- Dynamic column width calculation
- Terminal width detection and adaptation
- Color support for different file types
- Unicode and non-printable character handling

### util.c - Utility Functions
Supporting functions for file operations and information gathering:

**Core Functions:**
- `fmtentry()`: Format individual file entries
- `fmtinum()`: Format inode numbers
- `fmtmode()`: Format file permissions
- `fmtsize()`: Format file sizes (with human-readable support)
- `fmttime()`: Format timestamps
- `getname()`: User and group name resolution

### cmp.c - Comparison Functions
File comparison and sorting implementations:

**Sorting Functions:**
- `namecmp()`: Alphabetical by name
- `revnamecmp()`: Reverse alphabetical
- `timecmp()`: By modification time
- `sizcmp()`: By file size
- `extcmp()`: By file extension
- `vercmp()`: Version number sorting

### extern.h - External Declarations
Function prototypes and external variable declarations for modular organization.

## Build Configuration

### Makefile
```makefile
PROG=    ls
.include <bsd.prog.mk>
```

**Build Process:**
- Standard MINIX BSD make infrastructure
- Links against standard C library (libc)
- FTS library support for directory traversal
- No special compilation flags required
- Cross-platform compilation support

## Implementation Details

### File Tree Traversal (FTS)
Advanced directory tree traversal using the FTS library:
- Efficient handling of large directory hierarchies
- Circular link detection and prevention
- Multiple traversal strategies
- Memory-efficient processing

### Display Formats
Multiple output formats supported:
1. **Single Column (-1)**: One entry per line
2. **Long Format (-l)**: Detailed file information
3. **Columns (-C)**: Multi-column output
4. **Stream**: Continuous output for performance
5. **Comma-separated (-m)**: CSV-style output

### Sorting System
Multi-level sorting with various criteria:
- Alphabetical (default)
- Modification time (-t)
- Access time (-u)
- Status change time (-c)
- File size (-S)
- File extension (-X)
- Version number (-v)

### Column Calculation
Dynamic column width determination:
- Terminal width detection
- Maximum file name length calculation
- Optimal column count computation
- Field width calculations for long format

## MINIX Integration

### System Dependencies
- VFS (Virtual File System) for file system operations
- MINIX user/group database for ownership information
- Terminal interface for output formatting
- Process permission system for file access

### Service Interaction
- Uses VFS for all file system queries
- Integrates with MINIX authentication system
- Respects MINIX file permission model
- Terminal capability detection for optimal display

## RISC-V 64-bit Considerations

### Architecture-Specific Notes
- No RISC-V specific code required (portable implementation)
- 64-bit inode numbers fully supported
- Large file size handling with 64-bit arithmetic
- Efficient directory traversal on RISC-V memory architecture

### Performance Characteristics
- Memory-mapped directory access benefits from RISC-V architecture
- Large directory handling scales well with 64-bit addressing
- Sorting algorithms optimized for RISC-V instruction set
- Terminal output operations efficient on RISC-V

### Cross-Compilation
- Builds cleanly with riscv64-unknown-elf-gcc
- No architecture-specific compilation flags needed
- Standard NetBSD portability ensures RISC-V compatibility

## Advanced Features

### Color Support
- Configurable color schemes for different file types
- Terminal capability detection
- LS_COLORS environment variable support
- Customizable color definitions

### Time Display
- Multiple time formats (locale-specific)
- Recent vs. old file time display
- Nanosecond precision where supported
- Time zone handling

### Size Formatting
- Human-readable sizes (-h): K, M, G, T suffixes
- Block count display (-s)
- Comma-separated thousands
- SI vs. binary unit options

### File Type Indicators
- Type characters (-F): / for directories, * for executables
- Color coding for different types
- Special handling for symbolic links
- Device file major/minor number display

## Testing and Validation

### Test Coverage
- Basic directory listing functionality
- All display format options
- Sorting algorithm verification
- Terminal width adaptation
- Large directory handling
- Special file type display
- Color output validation

### Integration Tests
- Part of MINIX base system test suite
- Cross-architecture compatibility testing
- Terminal capability detection tests
- Standards compliance validation

## Usage Examples

```bash
# Basic listing
ls

# Long format with details
ls -l

# Show hidden files
ls -a

# Human-readable sizes
ls -lh

# Sort by modification time
ls -lt

# Recursive listing
ls -R

# Show inode numbers
ls -i

# Color output
ls --color=auto

# Multiple options combined
ls -laht --color=auto /home/user/

# Sort by size
ls -lS

# Show file flags
ls -lo
```

## Standards Compliance

The MINIX ls implementation follows:
- IEEE Std 1003.1 (POSIX.1)
- Single UNIX Specification (SUS)
- NetBSD compatibility standards
- BSD historical behavior for compatibility

## Performance Optimizations

### Directory Reading
- Efficient directory entry buffering
- Minimal system call overhead
- Optimized sorting algorithms
- Memory-efficient large directory handling

### Output Formatting
- Stream-based output for large listings
- Dynamic column calculation
- Optimized string formatting
- Terminal capability caching

### File Information Caching
- User/group name caching
- Terminal width caching
- Color capability caching
- Sort key pre-computation

## Development Notes

### Code Architecture
- Modular design with clear separation of concerns
- Consistent error handling throughout
- Extensible sorting and display systems
- Comprehensive code documentation

### Extensibility
- Easy addition of new sorting criteria
- Pluggable display format system
- Configurable output options
- Terminal-independent design

This implementation provides a comprehensive, standards-compliant ls utility that serves as the primary file system exploration tool in MINIX 3. The sophisticated display options, efficient directory traversal, and cross-platform compatibility make it suitable for both interactive use and system administration across all MINIX architectures, including RISC-V 64-bit.