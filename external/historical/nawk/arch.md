# MINIX 3 Awk Pattern Processing Language Architecture

## Overview

The `/external/historical/nawk` directory contains the classic nawk (new awk) pattern scanning and processing language implementation. This provides a comprehensive text processing and report generation system with pattern matching, field processing, arithmetic operations, and programming constructs, making it essential for log analysis, data extraction, report generation, and automated text processing in the MINIX environment.

## Purpose and Functionality

### Core Processing Services
- **Pattern Scanning**: Line-by-line pattern matching and processing
- **Field Processing**: Automatic field splitting and manipulation
- **Text Transformation**: String manipulation and text conversion
- **Report Generation**: Formatted output and statistical reporting
- **Data Extraction**: Structured data extraction from text files

### Advanced Features
- **Programming Constructs**: Variables, arrays, functions, and control flow
- **Regular Expressions**: Full regular expression pattern matching
- **Arithmetic Operations**: Numerical computation and statistical analysis
- **File I/O**: Multiple file input/output and redirection
- **String Manipulation**: Comprehensive string processing functions

## Source Code Architecture

### Core Components (from dist directory)

#### Main Interpreter Engine
- **Purpose**: Program initialization and main processing loop
- **Key Functions**:
  - `main()`: Entry point and command-line processing
  - `awk_init()`: Interpreter initialization
  - `run()`: Main pattern-action execution loop
  - `getrec()`: Record (line) input processing
  - `program()`: AWK program compilation and execution
- **Features**: Comprehensive program parsing and execution engine

#### Pattern Matching System
- **Purpose**: Regular expression pattern compilation and matching
- **Key Functions**:
  - `reparse()`: Regular expression parsing and compilation
  - `execute()`: Pattern execution engine
  - `match()**: String pattern matching
  - `getline()**: Input line processing
  - `split()**: Field splitting and processing
- **Features**: Full POSIX regular expression support

#### Field Processing
- **Purpose**: Automatic field splitting and manipulation
- **Key Functions**:
  - `refield()`: Field splitting based on FS (field separator)
  - `getf()`: Field value retrieval
  - `setf()`: Field value setting
  - `field_split()**: Field separation logic
  - `field_assign()**: Field assignment processing
- **Features**: Configurable field separators and automatic splitting

#### Built-in Functions
- **Purpose**: Standard AWK built-in function implementation
- **Key Functions**:
  - `awk_length()`: String length calculation
  - `awk_substr()`: Substring extraction
  - `awk_index()`: String position finding
  - `awk_sub()`: Substitution function
  - `awk_gsub()`: Global substitution function
  - `awk_match()`: Pattern matching function
- **Features**: Comprehensive string and mathematical functions

#### I/O Management
- **Purpose**: Input/output redirection and file management
- **Key Functions**:
  - `redirection()`: File redirection processing
  - `awk_getopt()`: Command-line option parsing
  - `openfile()`: File opening and management
  - `closefile()`: File closure and cleanup
  - `print_()`: Output formatting and printing
- **Features**: Multiple file I/O with redirection support

#### Variable and Array Management
- **Purpose**: Variable storage and array processing
- **Key Functions**:
  - `setsym()`: Symbol (variable) setting
  - `getsym()`: Symbol retrieval
  - `array_init()`: Array initialization
  - `array_access()`: Array element access
  - `assoc_clear()`: Associative array clearing
- **Features**: Associative arrays and dynamic variable management

### AWK Program Structure

#### Program Execution Model
```c
struct awk_program {
    struct pattern_action *patterns;    /* Pattern-action pairs */
    struct function_def *functions;     /* User-defined functions */
    struct var_table *variables;        /* Variable symbol table */
    struct io_redir *redirections;      /* I/O redirections */
    
    /* Runtime state */
    Cell *fields;                       /* Field array (FNR, NF, etc.) */
    Cell *records;                      /* Record processing state */
    FILE *infile;                       /* Current input file */
    char *filename;                     /* Current filename */
    int lineno;                         /* Current line number */
};
```

#### Pattern-Action Pairs
- **BEGIN Patterns**: Execute before input processing
- **Regular Patterns**: Execute for matching input lines
- **END Patterns**: Execute after input processing
- **Range Patterns**: Execute for line ranges
- **Expression Patterns**: Execute for boolean expressions

#### Built-in Variables
- **Field Variables**: `$1, $2, $3, ...` - Individual fields
- **Record Variables**: `$0` - Entire record, `NF` - Number of fields
- **File Variables**: `FILENAME` - Current filename, `FNR` - File record number
- **System Variables**: `NR` - Total record number, `RS` - Record separator
- **Control Variables**: `FS` - Field separator, `OFS` - Output field separator

## Build Configuration

### Makefile Structure (bin directory)
```makefile
PROG= awk
SRCS= awkgram.c lex.c main.c parse.c proctab.c run.c \
      lib.c tran.c tree.c
MAN= awk.1

.include <bsd.prog.mk>
```

### Build Dependencies
- **Lex/Yacc**: Lexical analyzer and parser generators
- **Standard C Library**: Core C runtime support
- **Regular Expression Library**: System regex support

### Compilation Features
- **Parser Generation**: Automatic grammar compilation
- **Optimization**: Configurable optimization levels
- **Debug Support**: Optional debug output and tracing
- **POSIX Compliance**: Strict POSIX awk standard compliance

## Integration with MINIX System

### Message-Based Architecture
Awk integrates with MINIX through standard system interfaces:
- **VFS Integration**: File operations through virtual file system
- **Process Management**: Standard process lifecycle management
- **Memory Management**: Coordinates with VM for dynamic allocation

### Service Dependencies
- **File System Services**: Heavy reliance on VFS for file operations
- **Regular Expression Services**: System regex library integration
- **Memory Services**: Uses VM for associative array and variable storage

### MINIX-Specific Adaptations
- **Resource Management**: Careful resource allocation for embedded systems
- **Memory Efficiency**: Optimized for systems with limited memory
- **Error Handling**: Consistent with MINIX error reporting conventions
- **Performance**: Optimized for MINIX system call patterns

## Advanced Features

### Associative Arrays
- **String Keys**: Arbitrary string key support
- **Dynamic Growth**: Automatic array expansion
- **Memory Management**: Efficient memory allocation strategies
- **Iteration Support**: Comprehensive array traversal

### Regular Expression Engine
- **Pattern Compilation**: Efficient regex compilation and caching
- **Match Variables**: `RSTART`, `RLENGTH` for match information
- **Substitution**: Advanced substitution with backreferences
- **Split Function**: Pattern-based field splitting

### I/O Redirection
- **Input Redirection**: Multiple input file support
- **Output Redirection**: File and pipe output redirection
- **Append Mode**: File append capabilities
- **Pipe Support**: Command pipeline integration

### Mathematical Functions
- **Trigonometric**: `sin()`, `cos()`, `atan2()`, etc.
- **Logarithmic**: `log()`, `exp()`, `sqrt()`
- **Random Numbers**: `rand()`, `srand()`
- **Integer Functions**: `int()`, `rand()`

## RISC-V 64-bit Considerations

### Architecture Optimization
- **Memory Alignment**: Proper alignment for RISC-V memory requirements
- **Large Number Support**: Efficient 64-bit arithmetic operations
- **Performance Tuning**: RISC-V-specific algorithm optimizations
- **Cross-Platform Compatibility**: Maintains consistent behavior

### Platform-Specific Features
- **Memory Efficiency**: Optimized for embedded RISC-V systems
- **File System Integration**: Optimized for MINIX VFS on RISC-V
- **Performance Characteristics**: Tuned for RISC-V pipeline architecture
- **Resource Constraints**: Careful resource usage for constrained environments

### Testing and Validation
- **RISC-V Specific Tests**: Architecture-specific text processing validation
- **Performance Benchmarking**: Processing performance on RISC-V hardware
- **Memory Usage Analysis**: Memory consumption profiling on RISC-V
- **Compatibility Testing**: Cross-platform AWK compatibility verification

## Usage Examples

### Basic Text Processing
```bash
# Print all lines
awk '{print}' file.txt

# Print specific fields
awk '{print $1, $3}' file.txt

# Print lines matching pattern
awk '/pattern/ {print}' file.txt

# Print line numbers
awk '{print NR, $0}' file.txt
```

### Field Processing
```bash
# Print first field
awk '{print $1}' data.txt

# Print last field
awk '{print $NF}' data.txt

# Print field count
awk '{print NF}' data.txt

# Custom field separator
awk -F: '{print $1}' /etc/passwd
```

### Pattern Matching and Substitution
```bash
# Lines containing pattern
awk '/error/ {print}' logfile

# Case-insensitive matching
awk 'tolower($0) ~ /error/ {print}' logfile

# Substitution
awk '{gsub(/old/, "new"); print}' file.txt

# Pattern ranges
awk '/start/,/end/ {print}' file.txt
```

### Mathematical Operations
```bash
# Sum first column
awk '{sum += $1} END {print sum}' numbers.txt

# Average calculation
awk '{sum += $1; count++} END {print sum/count}' data.txt

# Maximum value
awk 'max < $1 {max = $1} END {print max}' data.txt

# Statistical analysis
awk '{sum+=$1; sumsq+=$1*$1} END {print "avg:", sum/NR, "std:", sqrt(sumsq/NR - (sum/NR)^2)}' data.txt
```

### Report Generation
```bash
# Formatted output
awk 'BEGIN {printf "%-10s %8s\n", "Name", "Score"} {printf "%-10s %8d\n", $1, $2}' scores.txt

# Multiple file processing
awk 'FNR==1 {print "\nFile:", FILENAME} {print $0}' *.txt

# Conditional formatting
awk '$3 > 80 {print $1, "PASS"; next} {print $1, "FAIL"}' grades.txt
```

### Advanced Programming
```bash
# Associative arrays
awk '{count[$1]++} END {for (item in count) print item, count[item]}' items.txt

# User-defined functions
awk 'function max(a,b) {return a>b ? a : b} {print max($1,$2)}' pairs.txt

# Complex pattern matching
awk 'BEGIN {IGNORECASE=1} /pattern/ && $2 > 100 {print}' data.txt

# Multi-line records
awk 'BEGIN {RS=""; FS="\n"} {print $1, NF}' multiline.txt
```

## Development and Debugging

### Debug Features
- **Verbose Parsing**: Detailed program parsing information
- **Execution Tracing**: Pattern-action execution tracking
- **Variable Inspection**: Variable value inspection and dumping
- **Performance Profiling**: Execution timing and performance metrics

### Error Handling
- **Syntax Errors**: Detailed AWK syntax error reporting
- **Runtime Errors**: Comprehensive runtime error handling
- **File Errors**: File I/O error detection and reporting
- **Regular Expression Errors**: Regex compilation and execution errors

### Testing Infrastructure
- **Program Validation**: AWK program syntax validation
- **Function Testing**: Built-in function correctness testing
- **Performance Testing**: Large file processing performance testing
- **Compatibility Testing**: POSIX compliance verification

## Performance Characteristics

### Scalability
- **Large Files**: Efficient processing of multi-megabyte files
- **Many Files**: Fast multi-file input processing
- **Complex Patterns**: Optimized complex pattern matching
- **Memory Efficiency**: Efficient associative array memory usage

### Optimization Features
- **Pattern Compilation**: Pre-compiled regular expressions
- **Hash Table Optimization**: Efficient associative array implementation
- **Streaming Processing**: Single-pass processing where possible
- **Memory Management**: Dynamic memory allocation optimization

### Benchmarking
- **Processing Speed**: Optimized for high-throughput text processing
- **Pattern Matching**: Efficient regular expression engine
- **Memory Usage**: Efficient associative array and variable storage
- **I/O Efficiency**: Optimized file I/O operations

## Integration with Text Processing Pipeline

### Common Combinations
```bash
# Extract and sum
awk '{print $2}' data.txt | paste -sd+ | bc

# Filter and process
grep "pattern" file.txt | awk '{sum += $1} END {print sum}'

# Complex pipeline
cat logfile | grep ERROR | awk '{count[$2]++} END {for (c in count) print c, count[c]}'

# Statistical analysis
awk '{print $1}' data.txt | sort | uniq -c | awk '{print $2, $1}'
```

### System Administration
```bash
# Process monitoring
ps aux | awk '$3 > 10 {print $2, $3, $11}'

# Disk usage analysis
df -h | awk '$5 > 80 {print $1, $5}'

# Log analysis
awk '$9 >= 400 {print $1, $7, $9}' access.log

# User management
awk -F: '$3 > 1000 {print $1}' /etc/passwd
```

### Development Workflow
```bash
# Code metrics
awk '/^int / || /^void / {functions++} END {print functions}' *.c

# Build analysis
make 2>&1 | awk '/error:/ {print NR ": " $0}'

# Test result processing
awk '/PASS/ {pass++} /FAIL/ {fail++} END {print "Pass:", pass, "Fail:", fail}' results.txt

# Documentation generation
awk '/^## / {print "- " substr($0, 4)}' README.md
```

This nawk implementation provides a comprehensive pattern processing and text analysis language with full POSIX compliance and MINIX-specific optimizations, making it an essential component of the text processing and data analysis toolkit, particularly optimized for RISC-V 64-bit systems.