# MINIX 3 mklocale Tool - Architecture Documentation

## Overview

The `tools/mklocale/` directory provides the `mklocale` utility, a critical build tool for generating locale database files from source locale definition files. This tool is essential for internationalization (i18n) support in MINIX 3, enabling the creation of culturally-specific formatting rules for dates, times, numbers, currencies, and character collation sequences across different languages and regions.

## Purpose and Role in Build System

`mklocale` compiles human-readable locale definition files (`.src`) into binary locale database files that can be efficiently loaded and used by applications at runtime. This tool processes complex cultural conventions including character classification, collation orders, date/time formatting patterns, monetary representations, and numeric formatting rules, ensuring that MINIX systems can adapt to local preferences and standards worldwide.

## Directory Structure

```
tools/mklocale/
├── Makefile                     # Build configuration for host tool
└── obj/                        # Build artifacts directory
```

## Source Architecture

### Primary Source Location
The actual `mklocale` source code resides in:
```
usr.bin/mklocale/
├── mklocaledb.c                # Main implementation (10KB)
├── lex.l                       # Lexical analyzer (5KB)
├── yacc.y                      # Grammar parser (23KB)
├── ldef.h                      # Locale definition structures
├── mklocale.1                  # Manual page (7KB)
├── Makefile                    # Standard NetBSD makefile
└── obj/                        # Object files
```

### Build Tool Integration
The `tools/mklocale/Makefile` serves as a wrapper that:
- Defines `HOSTPROGNAME=${_TOOL_PREFIX}mklocale`
- References `HOST_SRCDIR=usr.bin/mklocale`
- Includes `Makefile.host` for host tool building

## Core Functionality

### Locale Database Generation Process

1. **Source Parsing** - Read and validate locale definition source files
2. **Character Classification** - Process character categories and conversions
3. **Collation Definition** - Build sorting and comparison rules
4. **Formatting Patterns** - Compile date, time, and number formats
5. **Monetary Conventions** - Process currency symbols and formatting
6. **Binary Generation** - Create optimized locale database files
7. **Validation** - Verify database integrity and consistency

### Supported Locale Categories

**Character Handling:**
- **LC_CTYPE** - Character classification and case conversion
- **LC_COLLATE** - String collation and sorting rules
- Character encoding mappings
- Multibyte character support

**Cultural Conventions:**
- **LC_TIME** - Date and time formatting patterns
- **LC_NUMERIC** - Numeric formatting (decimal point, thousands separator)
- **LC_MONETARY** - Currency formatting and symbols
- **LC_MESSAGES** - Message catalogs and error strings

**System Integration:**
- **LC_ALL** - Override all locale categories
- **LANG** - Default locale setting
- **LC_CTYPE** - Character type information

### Locale Source File Format

```
LC_CTYPE
# Character classification
cntrl   <U0000>-<U001F> <U007F>
lower   <U0061>-<U007A>
upper   <U0041>-<U005A>

LC_TIME
# Date/time formatting
abday   "Sun";"Mon";"Tue";"Wed";"Thu";"Fri";"Sat"
d_t_fmt "%a %b %e %H:%M:%S %Y"
```

## Implementation Details

### Parser Architecture (`yacc.y`)

**Grammar Components:**
- **Character Set Definitions** - LC_CTYPE specifications
- **Collation Rules** - LC_COLLATE sequences
- **Time Formats** - LC_TIME patterns
- **Numeric Formats** - LC_NUMERIC rules
- **Monetary Formats** - LC_MONETARY specifications

**Key Productions:**
```yacc
localedef: charset_map collating_rules time_format numeric_format monetary_format
charset_map: LC_CTYPE charmap_definitions
char_map: CHARMAP char_symbol char_encoding
```

### Lexical Analysis (`lex.l`)

**Token Categories:**
- **Keywords** - Locale category names
- **Symbols** - Character specifications
- **Strings** - Format patterns and literals
- **Numbers** - Code point values and ranges
- **Operators** - Range and set operators

**Character Encoding Support:**
- Unicode code point notation (`<UXXXX>`)
- Octal character specifications
- Escape sequence handling
- Multibyte character recognition

### Data Structures (`ldef.h`)

**Locale Database Components:**
```c
struct locale_data {
    struct lc_ctype    *ctype;      /* Character classification */
    struct lc_collate  *collate;    /* Collation rules */
    struct lc_time     *time;       /* Time formatting */
    struct lc_numeric  *numeric;    /* Numeric formatting */
    struct lc_monetary *monetary;   /* Monetary formatting */
};
```

**Character Classification:**
```c
struct lc_ctype {
    uint32_t *ctype_tab;            /* Character type table */
    uint32_t *tolower_tab;          /* Upper to lower case */
    uint32_t *toupper_tab;          /* Lower to upper case */
    size_t    tab_size;             /* Table dimensions */
};
```

## Cross-Compilation Architecture

### Host Tool Design
`mklocale` is built as a **host tool**:
- Compiled for the development host architecture
- Executed during the build process
- Not installed on the target MINIX system

### Build System Integration

**Makefile Configuration:**
```makefile
HOSTPROGNAME= ${_TOOL_PREFIX}mklocale
HOST_SRCDIR= usr.bin/mklocale
.include "${.CURDIR}/../Makefile.host"
```

**Lex/Yacc Integration:**
- Uses host `lex` and `yacc` tools during build
- Generates platform-independent parsers
- Supports cross-platform compilation
- Handles host/target separation correctly

### RISC-V Cross-Compilation Support

**Locale Database Portability:**
- Generated locale databases are architecture-independent
- Binary format works across all supported platforms
- No special handling needed for RISC-V 64-bit
- Endianness handled automatically

**Build Workflow for RISC-V:**
```bash
# Host builds mklocale
./build.sh -m evbriscv64 tools

# Use host mklocale for RISC-V locale databases
nbmklocale locale.db locale.src

# Database works on RISC-V target
```

## Locale Source Processing

### Character Set Definition

**Unicode Support:**
```
LC_CTYPE
cntrl   <U0000>-<U001F> <U007F>
lower   <U0061>-<U007A>
upper   <U0041>-<U005A>
digit   <U0030>-<U0039>
```

**Multibyte Character Handling:**
- UTF-8 sequence recognition
- Wide character support
- State-dependent encoding
- Shift sequence processing

### Collation Rules

**Primary Sort Order:**
```
LC_COLLATE
order_start forward
<U0041> <U0061>   # 'A' and 'a' equivalent
<U0042>           # 'B' follows 'A'
order_end
```

**Secondary Sorting:**
- Case distinctions
- Accent ordering
- Locale-specific priorities
- Custom weight assignments

### Time Formatting Patterns

**Date/Time Templates:**
```
LC_TIME
d_t_fmt "%a %b %e %H:%M:%S %Y"
d_fmt   "%m/%d/%y"
t_fmt   "%H:%M:%S"
am_pm   "AM";"PM"
```

**Cultural Variations:**
- Different calendar systems
- Time zone conventions
- Week start preferences
- Holiday definitions

## Binary Database Format

### File Structure

**Header Section:**
```c
struct locale_file_header {
    uint32_t magic;              /* File identification */
    uint32_t version;            /* Format version */
    uint32_t category_mask;      /* Enabled categories */
    uint32_t data_offset;        /* Start of data section */
};
```

**Category Sections:**
- LC_CTYPE tables and mappings
- LC_COLLATE weight tables
- LC_TIME format strings
- LC_NUMERIC separators
- LC_MONETARY symbols

### Optimization Features

**Memory Efficiency:**
- Compressed character tables
- Shared string pools
- Delta encoding for ranges
- Minimal redundancy

**Access Performance:**
- Direct table lookup
- Binary search for ranges
- Cached conversions
- Optimized string operations

## Build Dependencies

### Required Components
- Host C compiler
- Lex lexical analyzer generator
- Yacc parser generator
- `libnbcompat` for platform compatibility
- Standard C library functions

### Optional Features
- Unicode database support
- Large character set handling
- Complex collation rules
- Multiple calendar systems

## Testing and Validation

### Database Validation
- Magic number verification
- Category consistency checks
- Character table integrity
- Collation rule validation

### Cross-Platform Testing
- Locale database portability
- Character encoding verification
- Collation result consistency
- RISC-V target validation

## Performance Characteristics

### Processing Efficiency
- Linear parsing complexity for source files
- Efficient table generation algorithms
- Optimized binary format for runtime access
- Minimal memory footprint

### Runtime Performance
- Fast character classification
- Optimized collation operations
- Efficient time formatting
- Cached locale data access

## Internationalization Integration

### Build Pipeline
1. **Locale Source Creation** - Developers or translators create `.src` files
2. **mklocale Processing** - Build system invokes `mklocale` for each locale
3. **Database Installation** - Binary databases installed in target system
4. **Runtime Loading** - Applications load appropriate locale data

### Locale Selection
```c
setlocale(LC_ALL, "en_US.UTF-8");    /* US English */
setlocale(LC_TIME, "de_DE.UTF-8");   /* German time format */
setlocale(LC_MONETARY, "ja_JP.UTF-8"); /* Japanese currency */
```

### MINIX-Specific Usage
- System utility localization
- Error message formatting
- Administrative tool output
- Development environment support

## Character Encoding Support

### Unicode Integration
- UTF-8 processing throughout
- Unicode character database
- Normalization support
- Case folding rules

### Legacy Encodings
- ISO-8859 family support
- Windows code page compatibility
- EBCDIC for mainframe integration
- Asian multibyte encodings

## Development Guidelines

### Locale Source Best Practices
1. Use standard POSIX locale categories
2. Follow Unicode character naming conventions
3. Provide complete character coverage
4. Test collation with sample data
5. Document cultural conventions

### Database Generation
1. Validate source file syntax
2. Check character set completeness
3. Verify collation consistency
4. Test with sample applications
5. Validate binary format

### Debugging and Troubleshooting
- Use verbose parsing mode
- Check character encoding consistency
- Validate collation sequences
- Test locale database loading
- Monitor runtime locale usage

## RISC-V Specific Considerations

### Target Architecture Compatibility
- Locale databases are architecture-neutral
- No endianness issues with current format
- No word size dependencies
- Direct portability to RISC-V 64-bit

### Cross-Compilation Workflow
```bash
# Build mklocale for host
./build.sh -m evbriscv64 tools

# Generate locale databases
nbmklocale /usr/share/locale/en_US/LC_CTYPE en_US.src
nbmklocale /usr/share/locale/en_US/LC_TIME en_US_time.src
nbmklocale /usr/share/locale/en_US/LC_NUMERIC en_US_numeric.src
```

### Embedded System Considerations
- Minimal locale database sizes
- Essential categories only
- Compressed storage options
- Runtime memory efficiency

This tool is fundamental to MINIX's internationalization capabilities, enabling the creation of culturally-adapted systems that respect local conventions for character handling, sorting, time representation, and numeric formatting while maintaining full compatibility with the RISC-V 64-bit target architecture.