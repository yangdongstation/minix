# MINIX 3 makewhatis Tool - Architecture Documentation

## Overview

The `tools/makewhatis/` directory provides the `makewhatis` utility, a critical documentation tool for generating and maintaining the `whatis` database used by the `apropos`, `whatis`, and `man -k` commands. This tool processes manual pages across the entire MINIX system to create a comprehensive, searchable index of command descriptions, system calls, library functions, and file formats, enabling users to quickly discover relevant documentation.

## Purpose and Role in Build System

`makewhatis` serves as the backbone of MINIX's documentation discovery system. It scans manual page directories, extracts NAME section descriptions, and builds optimized database files that enable fast keyword-based searches. For cross-compilation scenarios, particularly RISC-V development, it ensures that documentation indices are correctly generated for the target system regardless of the host build environment.

## Directory Structure

```
tools/makewhatis/
├── Makefile                     # Build configuration for host tool
└── obj/                        # Build artifacts directory
```

## Source Architecture

### Primary Source Location
The actual `makewhatis` source code resides in:
```
libexec/makewhatis/
├── makewhatis.c                # Main implementation (26KB)
├── makewhatis.8                # Manual page (4KB)
├── Makefile                    # Standard NetBSD makefile
└── obj/                        # Object files
```

### Build Tool Integration
The `tools/makewhatis/Makefile` serves as a wrapper that:
- Defines `HOSTPROGNAME=${_TOOL_PREFIX}makewhatis`
- References `HOST_SRCDIR=libexec/makewhatis`
- Sets `CPPFLAGS+=-DNROFF="\"nbnroff\""` for nroff integration
- Includes `Makefile.host` for host tool building

## Core Functionality

### Manual Page Processing Pipeline

1. **Directory Scanning** - Recursively scan manual page directories
2. **File Identification** - Identify valid manual page files (`.1`, `.2`, `.3`, etc.)
3. **NAME Section Extraction** - Parse NAME sections for command descriptions
4. **Keyword Indexing** - Build searchable keyword databases
5. **Database Optimization** - Create efficient lookup structures
6. **Cross-Reference Generation** - Link related manual pages

### Manual Page Categories

**System Categories:**
- **Section 1** - User commands and utilities
- **Section 2** - System calls and kernel interfaces
- **Section 3** - Library functions and subroutines
- **Section 4** - Device drivers and special files
- **Section 5** - File formats and configuration files
- **Section 6** - Games and demonstrations
- **Section 7** - Miscellaneous information
- **Section 8** - System administration commands
- **Section 9** - Kernel development interfaces

### Database Format and Structure

**whatis Database Format:**
```
keyword\tdescription\tsection\tpath
command\tbrief description\t1\t/usr/share/man/man1/command.1
systemcall\tsystem call interface\t2\t/usr/share/man/man2/systemcall.2
libraryfunc\tlibrary function\t3\t/usr/share/man/man3/libraryfunc.3
```

**Index Organization:**
- Alphabetical keyword ordering
- Multiple entries per manual page
- Cross-reference linking
- Section-based categorization

## Implementation Details

### Main Components (`makewhatis.c`)

**Core Functions:**
- `main()` - Command-line processing and main control flow
- `process_directory()` - Scan and process manual page directories
- `process_file()` - Extract information from individual manual pages
- `parse_name_section()` - Parse NAME section for descriptions
- `build_database()` - Construct optimized keyword database
- `write_output()` - Generate final database files

**Data Structures:**
```c
struct man_entry {
    char *name;                 /* Command/function name */
    char *description;          /* Brief description */
    char *section;              /* Manual section number */
    char *path;                 /* File path */
    struct man_entry *next;     /* Linked list pointer */
};

struct keyword_index {
    char *keyword;              /* Searchable keyword */
    struct man_entry *entries;  /* Associated entries */
    struct keyword_index *next; /* Hash table chain */
};
```

### Manual Page Parsing

**NAME Section Extraction:**
```c
/* Extract NAME section from manual page */
static char *
extract_name_section(const char *content)
{
    char *name_start, *name_end;
    
    /* Find NAME section header */
    name_start = strstr(content, ".SH NAME");
    if (name_start == NULL)
        return NULL;
    
    /* Extract NAME content */
    name_start = strchr(name_start, '\n');
    name_end = strstr(name_start, ".SH ");
    
    return extract_section_content(name_start, name_end);
}
```

**Description Parsing:**
- Handle dash separators (`-` and `\-`)
- Process multiple name entries
- Extract brief descriptions
- Handle formatting macros

### NROFF Integration

**Preprocessor Configuration:**
```makefile
CPPFLAGS+= -DNROFF="\"nbnroff\""
```

**NROFF Processing:**
- Strip formatting commands
- Preserve structural information
- Handle macro expansions
- Maintain semantic content

## Cross-Compilation Architecture

### Host Tool Design
`makewhatis` is built as a **host tool**:
- Compiled for the development host architecture
- Executes during documentation generation
- Not installed on the target MINIX system

### Build System Integration

**Makefile Configuration:**
```makefile
HOSTPROGNAME= ${_TOOL_PREFIX}makewhatis
HOST_SRCDIR= libexec/makewhatis
CPPFLAGS+= -DNROFF="\"nbnroff\""
.include "${.CURDIR}/../Makefile.host"
```

**NROFF Dependency:**
- Uses `${_TOOL_PREFIX}nroff` for formatting
- Integrates with host toolchain
- Supports cross-platform manual processing
- Handles different nroff implementations

### RISC-V Cross-Compilation Support

**Documentation Generation for RISC-V:**
- Processes RISC-V specific manual pages
- Generates indices for cross-compiled documentation
- Handles architecture-specific commands
- Supports embedded system documentation

**Target Documentation Pipeline:**
```bash
# Build makewhatis for host
./build.sh -m evbriscv64 tools

# Generate whatis database for RISC-V target
nbmakewhatis /usr/share/man
nbmakewhatis /usr/X11R6/man
nbmakewhatis /usr/local/man
```

## Manual Page Processing Details

### File Discovery Algorithm

**Directory Traversal:**
```c
static void
scan_man_directories(const char *path)
{
    DIR *dirp;
    struct dirent *dp;
    
    dirp = opendir(path);
    while ((dp = readdir(dirp)) != NULL) {
        if (is_man_file(dp->d_name)) {
            process_man_file(dp->d_name);
        } else if (is_directory(dp->d_name)) {
            scan_man_directories(dp->d_name);
        }
    }
    closedir(dirp);
}
```

**File Type Recognition:**
- `.1` through `.9` extensions
- `.man` suffix support
- Compressed files (`.gz`, `.Z`)
- Catman preformatted pages

### NAME Section Analysis

**Multi-Entry Handling:**
```
.SH NAME
command1, command2, command3 \- brief description
.SH SYNOPSIS
```

**Description Extraction:**
- Split on comma separators
- Handle "and" conjunctions
- Process parenthetical notes
- Extract everything after dash

**Keyword Generation:**
- Primary name from filename
- Secondary names from NAME section
- Cross-reference aliases
- Section-specific indexing

### Database Construction

**Hash Table Implementation:**
```c
#define HASH_SIZE 1024

static struct keyword_index *
hash_keyword(const char *keyword)
{
    unsigned int hash = 0;
    const char *p;
    
    for (p = keyword; *p != '\0'; p++) {
        hash = (hash << 5) + *p;
    }
    
    return &hash_table[hash % HASH_SIZE];
}
```

**Collision Resolution:**
- Linked list chaining
- Case-insensitive comparison
- Duplicate entry prevention
- Alphabetical ordering within chains

## Database Optimization

### Storage Efficiency

**String Interning:**
- Shared description storage
- Common keyword pooling
- Path name compression
- Section string optimization

**Memory Management:**
- Dynamic allocation for variable data
- String pooling for duplicates
- Efficient hash table sizing
- Garbage collection during processing

### Lookup Performance

**Search Optimization:**
- Case-insensitive keyword matching
- Prefix search support
- Regular expression integration
- Fuzzy matching capabilities

**Access Patterns:**
- Direct hash table access
- Binary search for ranges
- Cached frequently-used entries
- Sequential scan fallback

## Command-Line Interface

### Basic Usage
```bash
makewhatis [-a] [-v] [-p] [manpath]
```

**Options:**
- `-a` - Process all manual page directories
- `-v` - Verbose operation
- `-p` - Use catman preformatted pages
- `manpath` - Specific manual page directory

### Advanced Options
```bash
makewhatis [-n nroff] [-o output] [-e encoding] [directories...]
```

**Advanced Options:**
- `-n nroff` - Specify nroff program path
- `-o output` - Specify output database file
- `-e encoding` - Character encoding for input files

### Typical Usage Patterns

**System-wide Database Update:**
```bash
makewhatis -a                    # Update all manual directories
```

**Single Directory Processing:**
```bash
makewhatis /usr/share/man/man1   # Update section 1 only
```

**Custom Output Location:**
```bash
makewhatis -o /tmp/whatis.db /usr/local/man
```

## Integration with Documentation System

### Database Files Generated

**Standard Locations:**
- `/usr/share/man/whatis` - Main whatis database
- `/usr/X11R6/man/whatis` - X11 documentation
- `/usr/local/man/whatis` - Local additions
- `/var/db/man.db` - Alternative location

**File Format:**
```
apropos\tsearch the whatis database for strings\t1\t/usr/share/man/man1/apropos.1
whatis\tsearch the whatis database for complete words\t1\t/usr/share/man/man1/whatis.1
man\tformat and display manual pages\t1\t/usr/share/man/man1/man.1
```

### Consumer Applications

**whatis Command:**
```bash
whatis keyword                   # Exact word match
```

**apropos Command:**
```bash
apropos keyword                  # Substring search
```

**man -k Integration:**
```bash
man -k keyword                   # Equivalent to apropos
```

## Cross-Platform Compatibility

### Host Platform Support

**Supported Hosts:**
- Linux distributions
- macOS/macOS X
- Solaris/OpenSolaris
- BSD systems
- Windows/Cygwin

**Platform-Specific Considerations:**
- Path separator handling
- Case sensitivity differences
- Character encoding variations
- File system capabilities

### Manual Page Format Compatibility

**Traditional Manual Pages:**
- BSD-style formatting
- System V conventions
- GNU extensions
- Platform-specific macros

**Modern Extensions:**
- XML-based manual pages
- HTML documentation
- PDF manual generation
- Hypertext cross-references

## Build Dependencies

### Required Components
- Host C compiler
- `libnbcompat` for platform compatibility
- nroff/troff processing capability
- Manual page source files

### Optional Features
- Compressed manual page support
- Unicode character handling
- Multiple encoding support
- Advanced search algorithms

## Testing and Validation

### Database Integrity Checks
- Magic number verification
- Entry count validation
- Keyword consistency checks
- Cross-reference verification

### Search Functionality Testing
- Exact match verification
- Case-insensitive search
- Substring matching
- Multi-keyword queries

### Cross-Platform Validation
- Database format consistency
- Character encoding verification
- Path name handling
- RISC-V target compatibility

## Performance Characteristics

### Processing Speed
- Linear complexity with manual page count
- Efficient file I/O operations
- Optimized string processing
- Minimal memory allocation

### Database Access Performance
- O(1) hash table lookup
- Efficient string comparison
- Fast alphabetical ordering
- Scalable to large manual collections

### Memory Usage
- Proportional to unique keyword count
- Shared string storage
- Efficient hash table sizing
- Temporary buffer management

## Development Guidelines

### Manual Page Authoring
1. Write clear, concise NAME sections
2. Use standard manual page formatting
3. Include relevant keywords
4. Provide accurate descriptions
5. Update whatis database after changes

### Database Maintenance
1. Regular updates after manual page changes
2. Validation of generated databases
3. Backup of previous versions
4. Cross-reference consistency checks
5. Performance monitoring

### Integration with Build System
1. Automated database generation
2. Dependency tracking for manual pages
3. Incremental updates for efficiency
4. Cross-compilation support
5. Installation path configuration

## RISC-V Specific Considerations

### Target Architecture Documentation
- RISC-V specific manual pages
- Architecture-specific commands
- Cross-compilation documentation
- Embedded system considerations

### Cross-Compilation Workflow
```bash
# Build makewhatis for host
./build.sh -m evbriscv64 tools

# Generate documentation index for RISC-V target
nbmakewhatis ${DESTDIR}/usr/share/man
nbmakewhatis ${DESTDIR}/usr/X11R6/man
```

### Embedded System Documentation
- Minimal manual page sets
- Essential command documentation
- Cross-reference optimization
- Space-efficient databases

This tool is essential for MINIX's documentation accessibility, ensuring that users can efficiently discover relevant manual pages across the entire system documentation hierarchy while maintaining full compatibility with cross-compilation environments and RISC-V target architectures.