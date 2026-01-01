# MINIX 3 mtree Tool - Architecture Documentation

## Overview

The `tools/mtree/` directory provides the `mtree` utility, a critical filesystem and software distribution tool for creating, verifying, and modifying directory hierarchies. This tool is essential for MINIX 3 system installation, package management, and filesystem integrity checking, enabling precise specification and control of file system contents, permissions, ownership, and metadata across diverse deployment scenarios.

## Purpose and Role in Build System

`mtree` serves multiple critical functions in the MINIX build and deployment ecosystem: it generates specification files that precisely describe directory hierarchies, verifies existing filesystems against specifications, creates directory structures from specifications, and modifies filesystem attributes to match desired states. For cross-compilation scenarios, particularly RISC-V development, it ensures that target filesystem images are correctly constructed regardless of the host build environment.

## Directory Structure

```
tools/mtree/
├── Makefile                     # Build configuration for host tool
└── obj/                        # Build artifacts directory
```

## Source Architecture

### Primary Source Location
The actual `mtree` source code resides in:
```
usr.sbin/mtree/
├── mtree.c                     # Main implementation (8KB)
├── mtree.h                     # Main header file (6KB)
├── mtree.8                     # Manual page (21KB)
├── spec.c                      # Specification file handling (23KB)
├── spec.h                      # Specification structures
├── compare.c                   # Filesystem comparison (16KB)
├── create.c                    # Directory creation (14KB)
├── verify.c                    # Verification operations (8KB)
├── getid.c                     # User/group ID resolution (10KB)
├── excludes.c                  # Exclusion pattern handling (3KB)
├── misc.c                      # Utility functions (7KB)
├── crc.c                       # CRC checksum calculation (7KB)
├── only.c                      # Selective processing (4KB)
├── specspec.c                  # Specification validation (7KB)
├── extern.h                    # External declarations
├── create.h                    # Creation interface
└── obj/                        # Object files
```

### Build Tool Integration
The `tools/mtree/Makefile` serves as a wrapper that:
- Defines `HOSTPROGNAME=${_TOOL_PREFIX}mtree`
- References `HOST_SRCDIR=usr.sbin/mtree`
- Optionally disables cryptographic hashes with `HOST_CPPFLAGS=-DNO_MD5 -DNO_RMD160 -DNO_SHA1 -DNO_SHA2`
- Includes `Makefile.host` for host tool building

## Core Functionality

### Specification File Format

**Basic Structure:**
```
# Comments start with #
/set type=file mode=0644 uid=0 gid=0
/unset type

.               type=dir mode=0755
    bin         type=dir mode=0755
        ls      type=file mode=0755 size=12345 \
                contents=./bin/ls
    etc         type=dir mode=0755
        passwd  type=file mode=0644 size=2345 \
                contents=./etc/passwd
```

**Advanced Features:**
- **Type Specifications** - file, dir, link, socket, fifo, block, char
- **Permission Control** - mode, uid, gid, uname, gname
- **Content Verification** - size, checksum, contents, flags, tags
- **Link Handling** - link, slink with target specification
- **Optional Keywords** - ignore, nochange, optional, silent
- **Comments and Whitespace** - Flexible formatting with comments

### Operational Modes

**Comparison Mode (`-c`):**
- Compare filesystem against specification
- Report differences and inconsistencies
- Verify checksums and permissions
- Generate difference reports

**Creation Mode (`-C`):**
- Create directory hierarchy from specification
- Set permissions and ownership
- Create files with specified content
- Establish symbolic links

**Verification Mode (`-f`):**
- Verify filesystem matches specification exactly
- Fail on any discrepancy
- Generate detailed error reports
- Suitable for automated testing

**Update Mode (`-u`):**
- Modify filesystem to match specification
- Change permissions and ownership
- Update file contents
- Create missing directories/files

### Key Specifications

**File Types:**
- `file` - Regular files with content verification
- `dir` - Directories with mode and ownership
- `link` - Hard links to specified targets
- `slink` - Symbolic links with path validation
- `socket` - UNIX domain sockets
- `fifo` - Named pipes
- `block` - Block special devices
- `char` - Character special devices

**Attribute Verification:**
- `mode` - File permissions (octal or symbolic)
- `uid`/`gid` - Numeric user/group identifiers
- `uname`/`gname` - User/group names (resolved at runtime)
- `size` - File size in bytes
- `link` - Target for hard links
- `slink` - Target for symbolic links

**Content Validation:**
- `contents` - File path for content comparison
- `cksum` - POSIX checksum value
- `md5`/`md5digest` - MD5 hash verification
- `sha1`/`sha1digest` - SHA-1 hash verification
- `sha256`/`sha256digest` - SHA-256 hash verification
- `ripemd160digest` - RIPEMD-160 hash verification

## Implementation Details

### Main Components (`mtree.c`)

**Core Functions:**
- `main()` - Command-line processing and mode selection
- `process_specification()` - Parse and validate specification files
- `compare_filesystem()` - Compare against existing filesystem
- `create_filesystem()` - Create new directory hierarchy
- `verify_filesystem()` - Verify exact filesystem match

**Data Structures:**
```c
struct node {
    char *name;                 /* File name */
    char *type;                 /* File type */
    char *path;                 /* Full path */
    struct stat sb;             /* File attributes */
    struct node *parent;        /* Parent directory */
    struct node *next;          /* Sibling list */
    struct node *child;         /* Child list */
    /* Additional attributes */
};

struct specification {
    struct node *root;          /* Root of specification tree */
    char *filename;             /* Specification file name */
    int flags;                  /* Processing flags */
};
```

### Specification Parsing (`spec.c`)

**Grammar Recognition:**
- Keyword identification and validation
- Attribute parsing and storage
- Path name processing
- Comment and whitespace handling
- Line continuation support

**Special Keyword Processing:**
```c
/* Process special keywords */
if (strcmp(keyword, "ignore") == 0) {
    node->flags |= F_IGNORE;
} else if (strcmp(keyword, "nochange") == 0) {
    node->flags |= F_NOCHANGE;
} else if (strcmp(keyword, "optional") == 0) {
    node->flags |= F_OPTIONAL;
}
```

### Filesystem Operations

**Comparison Algorithm:**
```c
static int
compare_node(struct node *spec, struct stat *st)
{
    /* Check file type */
    if (spec->type && !check_type(spec->type, st))
        return MTREE_TYPE_MISMATCH;
    
    /* Check permissions */
    if (spec->mode != (mode_t)-1 && spec->mode != st->st_mode)
        return MTREE_MODE_MISMATCH;
    
    /* Check ownership */
    if (spec->uid != (uid_t)-1 && spec->uid != st->st_uid)
        return MTREE_UID_MISMATCH;
    
    /* Check size for regular files */
    if (spec->size != (off_t)-1 && spec->size != st->st_size)
        return MTREE_SIZE_MISMATCH;
    
    return MTREE_MATCH;
}
```

**Creation Process:**
- Directory creation with proper modes
- File creation with content copying
- Symbolic link establishment
- Permission and ownership setting
- Checksum verification

## Cross-Compilation Architecture

### Host Tool Design
`mtree` is built as a **host tool**:
- Compiled for the development host architecture
- Executes during system construction and packaging
- Not installed on the target MINIX system

### Build System Integration

**Makefile Configuration:**
```makefile
HOSTPROGNAME= ${_TOOL_PREFIX}mtree
HOST_SRCDIR= usr.sbin/mtree
#HOST_CPPFLAGS= -DNO_MD5 -DNO_RMD160 -DNO_SHA1 -DNO_SHA2
.include "${.CURDIR}/../Makefile.host"
```

**Cryptographic Support:**
- Optional MD5, SHA-1, SHA-256, RIPEMD-160 checksums
- Configurable for minimal builds
- Host platform compatibility
- Cross-compilation considerations

### RISC-V Cross-Compilation Support

**Filesystem Specification for RISC-V:**
```
# RISC-V MINIX root filesystem specification
/set type=file mode=0644 uid=0 gid=0
.               type=dir mode=0755
    boot        type=dir mode=0755
        kernel  type=file mode=0755 \
                size=2097152 \
                contents=./obj.evbiscv64/minix/kernel/kernel
    sbin        type=dir mode=0755
        init    type=file mode=0755 \
                contents=./obj.evbiscv64/minix/commands/init/init
    usr         type=dir mode=0755
        bin     type=dir mode=0755
            ls  type=file mode=0755 \
                contents=./obj.evbiscv64/bin/ls/ls
```

**Target System Construction:**
- Build RISC-V filesystem images
- Verify cross-compiled binaries
- Create installation packages
- Generate distribution archives

## Command-Line Interface

### Basic Usage
```bash
mtree [-cCdDeilnPrsUux] [-f spec] [-p path] [-K keywords]
```

**Primary Options:**
- `-c` - Compare filesystem against specification
- `-C` - Create directory hierarchy from specification
- `-d` - Remove missing files during comparison
- `-D` - Debug mode with verbose output
- `-e` - Do not complain about missing specifications
- `-f spec` - Read specification from file
- `-i` - Indent output for readability
- `-K keywords` - Add keywords to check
- `-l` - Follow symbolic links
- `-n` - Do not actually modify filesystem
- `-p path` - Set root path for operations
- `-P` - Remove missing files during comparison
- `-r` - Remove files not in specification
- `-s` - Sort output
- `-U` - Modify filesystem to match specification
- `-x` - Do not cross mount points

### Advanced Options
```bash
mtree [-k keywords] [-X exclusions] [-R replacements]
```

**Advanced Features:**
- `-k keywords` - Remove default keywords
- `-X exclusions` - Exclude files matching patterns
- `-R replacements` - Replace path components
- `-w` - Treat warnings as errors
- `-W` - Specify warning categories

### Typical Usage Patterns

**Filesystem Verification:**
```bash
mtree -f specfile -p /target/directory
```

**Directory Creation:**
```bash
mtree -C -f specfile -p /new/directory
```

**Comparison with Updates:**
```bash
mtree -U -f specfile -p /existing/directory
```

**Incremental Updates:**
```bash
mtree -c -f specfile -p /directory -K mode,uid,gid
```

## Filesystem Specification Language

### Keyword Categories

**File Type Keywords:**
```
type=file     # Regular file
type=dir      # Directory
type=link     # Hard link
type=slink    # Symbolic link
type=socket   # UNIX domain socket
type=fifo     # Named pipe
type=block    # Block device
type=char     # Character device
```

**Permission Keywords:**
```
mode=0755     # Octal permission bits
mode=u+rx,g+rx,o+rx  # Symbolic permissions
uid=0         # Numeric user ID
gid=0         # Numeric group ID
uname=root    # User name (resolved at runtime)
gname=wheel   # Group name (resolved at runtime)
```

**Content Keywords:**
```
size=12345            # File size in bytes
cksum=1234567890      # POSIX checksum
md5digest=abc123...   # MD5 hash (128 bits)
sha1digest=def456...  # SHA-1 hash (160 bits)
sha256digest=ghi789... # SHA-256 hash (256 bits)
```

**Link Keywords:**
```
link=/path/to/target     # Hard link target
slink=/path/to/target    # Symbolic link target
```

**Behavior Keywords:**
```
ignore       # Ignore this entry during comparison
nochange     # Do not modify this entry
optional     # Do not complain if entry is missing
silent       # Suppress messages about this entry
```

### Set/Unset Mechanism

**Global Settings:**
```
/set type=file mode=0644 uid=0 gid=0
    file1    # Inherits file type, mode 0644, uid=0, gid=0
    file2    # Inherits same attributes
/unset mode # No longer inherit mode setting
    file3    # Inherits type, uid=0, gid=0, but not mode
```

**Conditional Processing:**
- Default attribute inheritance
- Selective attribute override
- Batch attribute modification
- Flexible specification reuse

## Advanced Features

### Checksum and Hash Support

**Available Algorithms:**
- **POSIX cksum** - Traditional 32-bit checksum
- **MD5** - 128-bit MD5 hash (if enabled)
- **SHA-1** - 160-bit SHA-1 hash (if enabled)
- **SHA-256** - 256-bit SHA-2 hash (if enabled)
- **RIPEMD-160** - 160-bit RIPEMD hash (if enabled)

**Usage in Specifications:**
```
bin/ls  type=file mode=0755 size=12345 \
      md5digest=d41d8cd98f00b204e9800998ecf8427e
etc/passwd  type=file mode=0644 size=2345 \
            sha256digest=e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
```

### Exclusion Patterns

**Pattern Matching:**
```bash
mtree -c -f specfile -X "*.tmp,*.bak,*~"
```

**Regular Expressions:**
```bash
mtree -c -f specfile -X "regex:.*\.o$"