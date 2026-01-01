# MINIX 3 pax Tool - Architecture Documentation

## Overview

The `tools/pax/` directory provides the `pax` utility, a comprehensive portable archive interchange tool that serves as the universal archiver for MINIX 3. This tool combines the functionality of traditional `tar`, `cpio`, and other archive formats into a single, standardized interface, enabling seamless archive creation, extraction, and manipulation across different UNIX systems and archive formats. For cross-compilation scenarios, particularly RISC-V development, it ensures that archive operations work consistently across diverse development and deployment environments.

## Purpose and Role in Build System

`pax` serves as MINIX's primary archiving utility, providing standardized archive handling for software distribution, backup operations, and system installation. It supports multiple archive formats (ustar, cpio, SVR4 tar), various compression methods, and cross-platform compatibility, making it essential for packaging MINIX distributions, creating installation media, and managing filesystem backups in heterogeneous computing environments.

## Directory Structure

```
tools/pax/
├── Makefile                     # Build configuration for host tool
└── obj/                        # Build artifacts directory
```

## Source Architecture

### Primary Source Location
The actual `pax` source code resides in:
```
bin/pax/
├── pax.c                       # Main implementation (17KB)
├── pax.h                       # Main header file (11KB)
├── pax.1                       # Manual page (33KB)
├── cpio.c                      # CPIO format support (29KB)
├── cpio.h                      # CPIO format definitions
├── tar.c                       # TAR format support (36KB)
├── tar.h                       # TAR format definitions
├── ar_io.c                     # Archive I/O operations (41KB)
├── ar_subs.c                   # Archive subsystem (36KB)
├── buf_subs.c                  # Buffer management (28KB)
├── file_subs.c                 # File operations (31KB)
├── ftree.c                     # File tree processing (21KB)
├── ftree.h                     # File tree interface
├── gen_subs.c                  # General subroutines (10KB)
├── getoldopt.c                 # Option parsing compatibility
├── options.c                   # Command-line options (47KB)
├── options.h                   # Option definitions
├── pat_rep.c                   # Pattern matching (29KB)
├── pat_rep.h                   # Pattern interface
├── sel_subs.c                  # Selection subroutines (14KB)
├── sel_subs.h                  # Selection interface
├── tables.c                    # Translation tables (38KB)
├── tables.h                    # Table definitions
├── extern.h                    # External declarations
├── dumptar.c                   # Tar format dumping
└── obj/                        # Object files
```

### Build Tool Integration
The `tools/pax/Makefile` serves as a wrapper that:
- Defines `HOSTPROGNAME=${_TOOL_PREFIX}pax`
- References `HOST_SRCDIR=bin/pax`
- Includes `Makefile.host` for host tool building

## Core Functionality

### Archive Format Support

**USTAR Format (POSIX.1-1988):**
- Standard Unix tape archive format
- 512-byte block structure
- Pathname length up to 100 characters
- UID/GID storage
- Modification times
- Checksum validation

**SVR4 Tar Format:**
- Extended tar format with additional features
- Long pathname support
- Extended header information
- Sparse file handling
- Enhanced metadata storage

**CPIO Formats:**
- Binary cpio (traditional format)
- ODc format (portable ASCII)
- SVR4 format with extended attributes
- CRC format with checksum validation

**BCP Format:**
- Binary interchange format
- Efficient binary representation
- Cross-platform compatibility

### Operational Modes

**Archive Creation (`-w`):**
- Create archives from file lists
- Support for multiple archive formats
- Incremental backup capabilities
- Compression integration
- Metadata preservation

**Archive Extraction (`-r`):**
- Extract archives to filesystem
- Selective file extraction
- Pathname modification
- Ownership and permission handling
- Overwrite protection

**Archive Listing (`-v`):**
- List archive contents
- Detailed file information
- Format-specific details
- Selective listing
- Verification operations

**Copy Mode (`-rw`):**
- Copy directory hierarchies
- Preserve file attributes
- Cross-filesystem copying
- Symbolic link handling
- Device file support

## Implementation Details

### Main Components (`pax.c`)

**Core Functions:**
- `main()` - Command-line processing and mode selection
- `pax_archive()` - Archive creation mode
- `pax_extract()` - Archive extraction mode
- `pax_list()` - Archive listing mode
- `pax_copy()` - File copy mode
- `signal_init()` - Signal handling setup

**Data Structures:**
```c
struct archive {
    char *name;                 /* Archive name */
    int fd;                     /* File descriptor */
    int format;                 /* Archive format */
    int compress;               /* Compression type */
    off_t size;                 /* Archive size */
    time_t mtime;               /* Modification time */
};

struct file_entry {
    char *name;                 /* File name */
    char *ln_name;              /* Link target */
    struct stat sb;             /* File attributes */
    int flags;                  /* Processing flags */
    struct file_entry *next;    /* Linked list */
};
```

### Archive Format Handling

**Format Detection:**
```c
static int
detect_format(int fd)
{
    char magic[6];
    
    if (read(fd, magic, sizeof(magic)) != sizeof(magic))
        return -1;
    
    if (memcmp(magic, "070707", 6) == 0)
        return F_CPIO;
    if (memcmp(magic, "ustar", 5) == 0)
        return F_USTAR;
    if (memcmp(magic, "\0" "ustar", 6) == 0)
        return F_TAR;
    
    return -1;
}
```

**Format-Specific Operations:**
- **CPIO**: Binary, ASCII, and CRC variants
- **TAR**: Standard and extended formats
- **USTAR**: POSIX standard format
- **BCP**: Binary interchange format

### TAR Format Implementation (`tar.c`)

**Header Structure:**
```c
struct tar_header {
    char name[100];             /* File name */
    char mode[8];               /* File mode */
    char uid[8];                /* User ID */
    char gid[8];                /* Group ID */
    char size[12];              /* File size */
    char mtime[12];             /* Modification time */
    char checksum[8];           /* Header checksum */
    char typeflag;              /* File type */
    char linkname[100];         /* Link target */
    char magic[6];              /* Magic number */
    char version[2];            /* Version */
    char uname[32];             /* User name */
    char gname[32];              /* Group name */
    /* Extended fields */
};
```

**Block Processing:**
- 512-byte block alignment
- Record spanning multiple blocks
- End-of-archive markers
- Padding and alignment

### CPIO Format Implementation (`cpio.c`)

**Binary CPIO Header:**
```c
struct cpio_header {
    short h_magic;              /* Magic number */
    short h_dev;                /* Device number */
    short h_ino;                /* Inode number */
    short h_mode;               /* File mode */
    short h_uid;                /* User ID */
    short h_gid;                /* Group ID */
    short h_nlink;              /* Link count */
    short h_rdev;               /* Device type */
    short h_mtime[2];           /* Modification time */
    short h_namesize;           /* Name length */
    short h_filesize[2];        /* File size */
};
```

**ASCII CPIO Format:**
```
070707 000000 000000 000000 000000 000000 000000 000000 000000 000000 000000 filename\0
```

## Cross-Compilation Architecture

### Host Tool Design
`pax` is built as a **host tool**:
- Compiled for the development host architecture
- Executes during package creation and installation
- Not installed on the target MINIX system

### Build System Integration

**Makefile Configuration:**
```makefile
HOSTPROGNAME= ${_TOOL_PREFIX}pax
HOST_SRCDIR= bin/pax
.include "${.CURDIR}/../Makefile.host"
```

**Cross-Platform Support:**
- Handles different host filesystem types
- Supports various archive formats
- Maintains portability across platforms
- Preserves file attributes correctly

### RISC-V Cross-Compilation Support

**Package Creation for RISC-V:**
```bash
# Create RISC-V distribution package
nbpax -w -f minix-riscv64.tar -C ${DESTDIR} .

# Extract RISC-V components
nbpax -r -f minix-riscv64.tar -C /target/riscv64

# List RISC-V package contents
nbpax -v -f minix-riscv64.tar
```

**Cross-Platform Archive Handling:**
- Create archives on host for RISC-V target
- Preserve RISC-V specific file attributes
- Handle cross-platform path conversions
- Maintain binary compatibility

## Advanced Features

### Pattern Matching and Selection

**Pattern Syntax (`pat_rep.c`):**
```c
/* Shell-style globbing patterns */
*.c             # All C source files
*.[ch]          # C source and header files
dir/*           # All files in directory
dir/**          # Recursive directory matching

/* Regular expression patterns */
regex:.*\.o$    # Object files
regex:^test.*   # Files starting with "test"
```

**Selection Logic:**
- Include/exclude pattern lists
- Regular expression support
- Case sensitivity options
- Path matching algorithms

### Compression Support

**Built-in Compression:**
- gzip format support
- compress format support
- bzip2 format support (if available)
- Automatic format detection

**Compression Integration:**
```bash
pax -w -z -f archive.tar.gz files...    # gzip compression
pax -w -Z -f archive.tar.Z files...     # compress compression
```

### Incremental Archives

**Modification Time Tracking:**
```bash
pax -w -T 1234567890 -f incremental.tar
```

**Differential Backups:**
- Time-based selection
- Size-based comparison
- Checksum-based verification
- Attribute-based filtering

## Command-Line Interface

### Basic Usage

**Archive Creation:**
```bash
pax -w -f archive.tar files...         # Create tar archive
pax -w -x cpio -f archive.cpio files... # Create cpio archive
pax -w -z -f archive.tar.gz files...   # Create compressed tar
```

**Archive Extraction:**
```bash
pax -r -f archive.tar                  # Extract tar archive
pax -r -f archive.cpio                 # Extract cpio archive
pax -r -s ',^old/,new/,' -f archive.tar # Extract with renaming
```

**Archive Listing:**
```bash
pax -v -f archive.tar                  # List tar contents
pax -v -f archive.cpio                 # List cpio contents
```

### Advanced Options

**Format Selection:**
```bash
pax -x ustar -w -f archive.tar        # USTAR format
pax -x cpio -w -f archive.cpio        # CPIO format
pax -x bcpio -w -f archive.bcpio      # Binary CPIO
```

**File Selection:**
```bash
pax -w -f archive.tar -T 1234567890   # Files modified after time
pax -w -f archive.tar -n pattern      # Include pattern
pax -w -f archive.tar -c pattern      # Exclude pattern
```

**Attribute Handling:**
```bash
pax -p e -r -f archive.tar            # Extract with all attributes
pax -p p -r -f archive.tar            # Preserve permissions only
pax -o listopt=:      # Extended listing format
```

## Pattern Matching and Selection

### Glob Pattern Support

**Basic Patterns:**
```
*.c              # All C files
*.[ch]           # C and header files
test*            # Files starting with "test"
*test*           # Files containing "test"
```

**Directory Patterns:**
```
dir/*            # Files in directory
dir/**           # Files in directory recursively
**/test*         # Test files in any directory
```

### Regular Expression Support

**Regex Patterns:**
```
regex:.*\.o$     # Object files
regex:^test.*    # Files starting with test
regex:.*[0-9].*  # Files containing digits
```

### Selection Logic

**Include/Exclude Lists:**
```bash
pax -w -f archive.tar -n '*.c' -n '*.h'      # Include C sources
pax -w -f archive.tar -c '*.o' -c '*.a'      # Exclude objects
pax -w -f archive.tar -n 'src/**' -c 'src/*.tmp' # Include src, exclude tmp
```

## Archive Format Details

### USTAR Format Specification

**Header Layout:**
```
Offset  Length  Field
0       100     name
100     8       mode
108     8       uid
116     8       gid
124     12      size
136     12      mtime
148     8       checksum
156     1       typeflag
157     100     linkname
257     6       magic
263     2       version
265     32      uname
297     32      gname
329     8       devmajor
337     8       devminor
345     155     prefix
```

**Type Flags:**
```
'0'     Regular file
'1'     Hard link
'2'     Symbolic link
'3'     Character device
'4'     Block device
'5'     Directory
'6'     FIFO (named pipe)
```

### CPIO Format Variants

**Binary CPIO:**
```
struct cpio_hdr {
    short h_magic;      /* 070707 */
    short h_dev;
    short h_ino;
    short h_mode;
    short h_uid;
    short h_gid;
    short h_nlink;
    short h_rdev;
    short h_mtime[2];
    short h_namesize;
    short h_filesize[2];
};
```

**ASCII CPIO:**
```
070707 000000 000000 000000 000000 000000 000000 000000 000000 000000 000000 filename\0
```

## Cross-Platform Compatibility

### Format Portability

**Cross-Platform Considerations:**
- Endianness handling
- Word size differences
- Path separator conversion
- Character encoding
- Timestamp representation

**Interoperability:**
- GNU tar compatibility
- BSD tar compatibility
- System V cpio compatibility
- POSIX standard compliance

### Host/Target Separation

**Build-time Usage:**
- Create distribution archives
- Package system components
- Generate installation media
- Backup build artifacts

**Runtime Usage:**
- Extract system archives
- Install software packages
- Perform system backups
- Migrate system components

## Performance Characteristics

### Archive Creation Performance

**I/O Optimization:**
- Block-aligned I/O operations
- Buffer size optimization
- Sequential access patterns
- Compression integration

**Memory Usage:**
- Streaming processing
- Minimal memory footprint
- Large file handling
- Buffer management

### Archive Extraction Performance

**Extraction Efficiency:**
- Direct file creation
- Attribute preservation
- Symbolic link handling
- Directory creation optimization

## Build Dependencies

### Required Components
- Host C compiler
- `libnbcompat` for platform compatibility
- Standard C library functions
- File system access capabilities

### Optional Features
- Compression library support (zlib, bzip2)
- Extended attribute support
- Large file support
- Advanced pattern matching

## Testing and Validation

### Format Compliance Testing

**USTAR Compliance:**
- POSIX.1-1988 validation
- Header checksum verification
- Block alignment testing
- Magic number validation

**CPIO Compliance:**
- Magic number verification
- Header format validation
- ASCII/binary format testing
- CRC validation (for CRC format)

### Cross-Platform Testing

**Interoperability Testing:**
- GNU tar compatibility
- BSD tar compatibility
- System V cpio compatibility
- RISC-V specific validation

**Archive Integrity:**
- Round-trip testing (create/extract/compare)
- Attribute preservation verification
- Large file handling
- Compression integration

## Development Guidelines

### Archive Creation Best Practices

**Format Selection:**
1. Use USTAR for maximum portability
2. Use CPIO for System V compatibility
3. Consider compression for large archives
4. Test cross-platform compatibility
5. Validate archive integrity

**Attribute Preservation:**
1. Preserve file permissions carefully
2. Handle symbolic links appropriately
3. Maintain timestamps accurately
4. Preserve ownership when possible
5. Handle special files correctly

### Integration with Build System

**Automated Archive Creation:**
```makefile
# Makefile integration
dist.tar: files...
    nbpax -w -f $@ $^

install: dist.tar
    nbpax -r -p e -f dist.tar -C ${DESTDIR}
```

**Cross-Compilation Support:**
```bash
# Create RISC-V distribution
nbpax -w -f minix-riscv64.tar -C ${DESTDIR} .

# Extract on target
nbpax -r -p e -f minix-riscv64.tar -C /
```

This tool is fundamental to MINIX's software distribution and system management capabilities, providing standardized archive handling that ensures consistent behavior across diverse platforms while maintaining full compatibility with cross-compilation environments and RISC-V target architectures.