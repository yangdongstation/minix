# MINIX 3 gencat Tool - Architecture Documentation

## Overview

The `tools/gencat/` directory provides the `gencat` utility, a critical build tool for generating National Language Support (NLS) message catalogs from source message files. This tool is essential for internationalization (i18n) support in MINIX 3, enabling the creation of localized system messages and error strings across different languages and cultural contexts.

## Purpose and Role in Build System

`gencat` compiles human-readable message source files (`.msg`) into binary message catalog files (`.cat`) that can be efficiently loaded and used by applications at runtime. This tool is particularly important for MINIX's userland utilities and system servers that need to provide localized feedback to users in their preferred language.

## Directory Structure

```
tools/gencat/
├── Makefile                     # Build configuration for host tool
└── obj/                        # Build artifacts directory
```

## Source Architecture

### Primary Source Location
The actual `gencat` source code resides in:
```
usr.bin/gencat/
├── gencat.c                     # Main implementation (20KB)
├── gencat.1                     # Manual page
├── Makefile                     # Standard NetBSD makefile
└── obj/                        # Object files
```

### Build Tool Integration
The `tools/gencat/Makefile` serves as a wrapper that:
- Defines `HOSTPROGNAME=${_TOOL_PREFIX}gencat`
- References `HOST_SRCDIR=usr.bin/gencat`
- Includes `Makefile.host` for host tool building

## Core Functionality

### Message Catalog Format
`gencat` processes message source files that contain:
- Message set definitions
- Individual message strings with numeric IDs
- Comments and metadata
- Escape sequences for special characters

### Binary Output Format
The generated `.cat` files contain:
- Magic number for format validation
- Message set directory
- Individual message offsets and lengths
- Compact string storage with shared suffix optimization

### Command-Line Interface
```bash
gencat [-l] catalog_file source_file...
gencat -v
```

**Options:**
- `-l` - List contents of existing catalog
- `-v` - Display version information

## Implementation Details

### Source Code Architecture (`gencat.c`)

**Main Components:**
- **Parser Module** - Lexical analysis and grammar parsing
- **Message Storage** - In-memory message organization
- **Binary Generation** - Catalog file creation
- **Error Handling** - Comprehensive error reporting

**Key Functions:**
- `main()` - Command-line processing and main loop
- `process_file()` - Process individual message source files
- `write_catalog()` - Generate binary catalog output
- `list_catalog()` - List existing catalog contents

**Data Structures:**
- `struct message` - Individual message representation
- `struct set` - Message set container
- `struct catalog` - Complete catalog structure

### Memory Management
- Dynamic allocation for message storage
- String interning for duplicate message reduction
- Efficient memory usage for large catalogs

## Cross-Compilation Architecture

### Host Tool Design
`gencat` is built as a **host tool**, meaning:
- Compiled for the development host architecture
- Executed during the build process
- Not installed on the target MINIX system

### Build System Integration

**Makefile.host Integration:**
```makefile
HOSTPROGNAME= ${_TOOL_PREFIX}gencat
HOST_SRCDIR= usr.bin/gencat
.include "${.CURDIR}/../Makefile.host"
```

**Cross-Platform Support:**
- Uses `libnbcompat` for host platform compatibility
- Supports Linux, macOS, Solaris, and other hosts
- Handles platform-specific path and file operations

### RISC-V Cross-Compilation

For RISC-V 64-bit builds (`evbriscv64`):
- `gencat` runs on the host (x86_64, etc.)
- Generates message catalogs for RISC-V target system
- Ensures catalog format compatibility across architectures
- Handles endianness and word size considerations

## Message Source File Format

### Syntax Specification
```
$set n                    # Define message set n
$del n                    # Delete message set n
$quote c                 # Define quote character
$                       # Empty line (ignored)
# comment               # Single-line comment
m text                  # Message m with text
text line 1\
text line 2             # Multi-line message
```

### Example Message File
```
$set 1
1 This is message 1
2 This is message 2
$quote "
3 "Quoted message"
$set 2
10 Error: File not found
11 Warning: Low memory
```

## Internationalization Integration

### Build Pipeline
1. **Source Messages** - Developers write message source files
2. **gencat Processing** - Build system invokes `gencat` for each locale
3. **Catalog Installation** - Binary catalogs installed in target system
4. **Runtime Loading** - Applications load appropriate catalogs

### Locale Support
- Multiple language support through separate catalogs
- Cultural convention handling (date, time, currency)
- Character encoding considerations (UTF-8, ISO-8859-1)

### MINIX-Specific Usage
- System server error messages
- Command-line utility output
- System administration tools
- Development tool messages

## Build Dependencies

### Required Components
- Host C compiler
- `libnbcompat` for platform compatibility
- Standard C library functions

### Optional Features
- Large file support for big message catalogs
- Unicode handling for international characters
- Compression for space optimization

## Testing and Validation

### Catalog Validation
- Magic number verification
- Message set consistency checks
- String offset validation
- Memory corruption detection

### Cross-Platform Testing
- Catalog format compatibility across hosts
- Endianness verification for target architectures
- RISC-V specific validation

## Performance Characteristics

### Processing Efficiency
- Linear parsing complexity
- Memory-efficient string storage
- Fast catalog loading at runtime

### Scalability
- Supports large message catalogs (1000+ messages)
- Efficient handling of multiple message sets
- Optimized for repeated access patterns

## Development Guidelines

### Message File Best Practices
1. Use descriptive message IDs
2. Keep messages concise and clear
3. Avoid hard-coding formatting
4. Provide context for translators
5. Use consistent terminology

### Integration with Build System
1. Add message source files to appropriate Makefiles
2. Define catalog generation rules
3. Set up locale-specific installation paths
4. Configure runtime catalog loading

### Debugging and Troubleshooting
- Use `-l` option to inspect catalog contents
- Check message source syntax carefully
- Verify catalog file permissions
- Monitor runtime catalog loading

## RISC-Specific Considerations

### Target Architecture Support
- Message catalog format is architecture-independent
- No special handling needed for RISC-V 64-bit
- Endianness handled automatically
- Word size agnostic design

### Cross-Compilation Workflow
```bash
# Host builds gencat
./build.sh -m evbriscv64 tools

# Use host gencat for RISC-V catalogs
nbgencat messages.cat messages.msg

# Catalog works on RISC-V target
```

This tool is essential for MINIX's internationalization support, enabling the creation of localized system software that can adapt to different languages and cultural contexts while maintaining compatibility across diverse development and target environments.