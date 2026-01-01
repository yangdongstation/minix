# MINIX 3 Architecture-Independent Data Files (/share)

## Overview

The `/share` directory contains architecture-independent data files that are shared across different hardware platforms and system architectures. This directory hosts system-wide data that doesn't depend on specific hardware, including message catalogs, manual pages, build system rules, locale information, and various system databases. It represents the portable, platform-neutral components of MINIX that remain consistent regardless of the underlying architecture.

## 本仓库实际包含/可选或上游组件

本仓库实际包含：以下为顶层条目示例（完整清单以目录实际内容为准）：`man/`, `mk/`, `locale/`, `terminfo/`, `Makefile`.
可选或上游组件：上文条目可能包含上游/可选组件示例，当前仓库未必包含全部。

## Directory Structure

### Internationalization and Localization

#### **`nls/`** - Native Language Support
- Message catalogs for different languages
- Translated system messages
- Locale-specific text data
- Error message translations
- System prompt localizations

#### **`i18n/`** - Internationalization Support
- Character set definitions
- Cultural convention data
- Locale-specific formatting rules
- Date/time format specifications
- Currency and number formatting

#### **`locale/`** - Locale Database
- Locale definition files
- Cultural convention databases
- Character classification tables
- Collation sequence definitions
- Monetary formatting rules

### Documentation and Manual Pages

#### **`man/`** - Manual Page System
- **Section 1** - User commands and utilities
- **Section 2** - System calls
- **Section 3** - Library functions
- **Section 4** - Device drivers and hardware
- **Section 5** - File formats and configuration
- **Section 6** - Games and entertainment
- **Section 7** - Miscellaneous information
- **Section 8** - System administration
- **Section 9** - Kernel internals

### Build System Infrastructure

#### **`mk/`** - Build System Rules and Definitions
- **`bsd.conversions.mk`** - BSD build conversions
- **`bsd.dep.mk`** - Dependency management
- **`bsd.files.mk`** - File handling rules
- **`bsd.host.mk`** - Host-specific build rules
- **`bsd.own.mk`** - Project-specific definitions
- **`bsd.prog.mk`** - Program building rules
- **`bsd.subdir.mk`** - Subdirectory handling
- **`bsd.sys.mk`** - System build configuration
- **`sys.mk`** - System-wide make definitions

### Terminal and Display Information

#### **`terminfo/`** - Terminal Capability Database
- Terminal type definitions
- Display capability descriptions
- Keyboard mapping information
- Screen control sequences
- Color and attribute support
- Cursor movement commands
- Function key definitions

#### **`misc/`** - Miscellaneous System Data
- System-wide configuration templates
- Default initialization files
- Sample configuration files
- System databases
- Reference data files

### Legal and Licensing Information

#### **`legal/`** - Legal and License Information
- Software license texts
- Copyright notices
- Legal disclaimers
- Attribution requirements
- License compliance information

## Key Files and Components

### Build System Integration
- **Makefile** - Master build configuration
- **Makefile.inc** - Common build rules

### Documentation System
- Manual page source files (`.1`, `.2`, `.3`, etc.)
- Documentation formatting macros
- Cross-reference databases
- Index generation tools

### Locale and Character Support
- Character encoding tables
- Locale definition files (`.src`)
- Collation sequence data
- Character classification rules
- Case conversion tables

## Integration with MINIX System

### Architecture Independence
All files in `/share` are designed to be:
- Platform-neutral
- Architecture-independent
- Portable across systems
- Consistent across installations

### System-Wide Availability
Shared data accessible to:
- All system utilities
- User applications
- System services
- Administrative tools

### Build System Integration
- Standardized build rules
- Consistent compilation flags
- Cross-platform compatibility
- Reproducible builds

## RISC-V 64-bit Considerations

### Platform Neutrality
Since `/share` contains architecture-independent data:
- No RISC-V specific modifications needed
- Consistent across all platforms
- Portable data representation
- Universal applicability

### Cross-Platform Consistency
Ensures consistent behavior:
- Same manual pages on all architectures
- Identical locale support
- Consistent terminal definitions
- Unified build system rules

### Internationalization Support
Full i18n support for RISC-V:
- Character set compatibility
- Locale-specific formatting
- Cultural convention support
- Multi-language capability

## Development Guidelines

### Data Format Standards
- Text-based formats where possible
- Standard encoding conventions
- Portable data representation
- Version control friendly formats

### Maintenance Standards
- Regular updates and corrections
- Consistency verification
- Cross-platform testing
- Documentation accuracy

### Internationalization
- Complete translation support
- Cultural sensitivity
- Locale-specific adaptations
- Unicode compliance

## Common Usage Patterns

### Manual Page Access
```bash
# View manual pages
man ls
man 2 open
man 5 passwd

# Search manual pages
apropos keyword
whatis command
```

### Locale and Character Support
```bash
# List available locales
locale -a

# Set system locale
export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8

# Character classification
locale charmap
```

### Terminal Configuration
```bash
# Set terminal type
export TERM=xterm-256color

# Query terminal capabilities
tput lines
tput cols
```

### Build System Usage
```bash
# Include build rules
.include <bsd.prog.mk>

# Use system definitions
PROG=myprogram
SRCS=myprogram.c
```

## Advanced Features

### Localization Support
- Multiple language support
- Cultural adaptation
- Locale-specific formatting
- Time zone handling

### Documentation System
- Comprehensive manual coverage
- Cross-referencing system
- Searchable documentation
- Regular updates

### Terminal Compatibility
- Extensive terminal database
- Legacy terminal support
- Modern terminal features
- Portable terminal applications

## Maintenance and Updates

### Documentation Updates
- Regular manual page updates
- New utility documentation
- Correction of errors
- Consistency improvements

### Internationalization Updates
- Translation updates
- New locale support
- Cultural convention updates
- Character set expansions

### Build System Evolution
- Improved build rules
- Cross-platform enhancements
- Performance optimizations
- New feature integration

## Directory Organization Examples

### Typical Structure
```
share/
├── man/
│   ├── man1/       # User commands
│   ├── man2/       # System calls
│   ├── man3/       # Library functions
│   └── ...
├── mk/
│   ├── bsd.prog.mk
│   ├── bsd.dep.mk
│   └── ...
├── terminfo/
│   ├── a/          # Terminals starting with 'a'
│   ├── b/          # Terminals starting with 'b'
│   └── ...
├── locale/
│   ├── en_US.UTF-8/
│   ├── de_DE.UTF-8/
│   └── ...
└── misc/
    ├── termcap
    ├── magic
    └── ...
```

### File Type Distribution
- **Documentation**: Manual pages, help files
- **Build data**: Makefiles, rules, templates
- **Locale data**: Character sets, collation
- **Terminal data**: Terminfo entries, capabilities
- **Legal data**: Licenses, copyright notices

The `/share` directory embodies MINIX's commitment to portability and internationalization, providing the foundation for consistent system behavior across different architectures while supporting diverse languages, cultures, and usage patterns. Its architecture-independent nature makes it essential for maintaining consistency in the MINIX ecosystem across all supported platforms, including the RISC-V 64-bit architecture.
