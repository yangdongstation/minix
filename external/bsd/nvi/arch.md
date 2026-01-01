# MINIX 3 Vi Text Editor Architecture (nvi)

## Overview

The `/external/bsd/nvi` directory contains the BSD nvi (new vi) text editor implementation, a comprehensive screen-oriented text editor derived from the classic Unix vi editor. This implementation provides full vi compatibility with modern extensions, multiple buffer support, sophisticated regular expression capabilities, and integration with the MINIX microkernel architecture, making it the primary text editing tool for system administration, development, and general text manipulation in the MINIX environment.

## Purpose and Functionality

### Core Editing Services
- **Screen-Oriented Editing**: Full-screen visual text editing
- **Modal Interface**: Command, insert, and visual modes
- **Buffer Management**: Multiple file and buffer support
- **Regular Expressions**: Advanced pattern matching and substitution
- **Macro System**: Complex command automation and scripting

### Advanced Features
- **Multiple Windows**: Split-window editing capabilities
- **Extended Regular Expressions**: Enhanced regex support beyond POSIX
- **Command History**: Command-line history and recall
- **File Recovery**: Crash recovery and backup systems
- **Internationalization**: Multi-byte character and locale support
- **Customization**: Extensive configuration and option system

## Source Code Architecture

### Core Components (from dist directory)

#### Main Editor Engine
- **Purpose**: Central editing loop and command processing
- **Key Functions**:
  - `main()`: Editor initialization and main loop
  - `editor()`: Core editing command dispatcher
  - `v_edit()`: Main editing function with mode management
  - `v_init()`: Editor state initialization
  - `v_end()`: Editor cleanup and termination
- **Features**: Comprehensive mode management and command routing

#### Screen Management
- **Purpose**: Terminal control and screen display management
- **Key Functions**:
  - `vs_paint()`: Screen painting and display update
  - `vs_line()`: Individual line display and formatting
  - `vs_column()`: Column and cursor position management
  - `vs_ex_resolve()**: Ex command line resolution
  - `vs_msg()**: Status line and message display
- **Features**: Sophisticated screen management with optimization

#### Buffer Management
- **Purpose**: Text buffer manipulation and file I/O
- **Key Functions**:
  - `db_append()`: Buffer text append operations
  - `db_delete()`: Buffer text deletion
  - `db_get()`: Buffer line retrieval
  - `db_set()`: Buffer line modification
  - `db_last()`: Buffer end position management
- **Features**: Efficient buffer management with undo support

#### Regular Expression Engine
- **Purpose**: Pattern matching and substitution system
- **Key Functions**:
  - `re_compile()`: Regular expression compilation
  - `re_exec()`: Pattern execution and matching
  - `re_sub()`: Substitution and replacement
  - `re_error()`: Regular expression error handling
- **Features**: Full POSIX regex support with extensions

#### Command Line Interface
- **Purpose**: Ex command line processing and execution
- **Key Functions**:
  - `ex_cmd()`: Ex command dispatcher
  - `ex_range()`: Command range parsing
  - `ex_at()`: Command execution at specific lines
  - `ex_subst()`: Substitution command implementation
  - `ex_global()`: Global command processing
- **Features**: Comprehensive ex command set with scripting

#### Search and Replace
- **Purpose**: Text search and replacement operations
- **Key Functions**:
  - `f_search()`: Forward search implementation
  - `b_search()`: Backward search implementation
  - `s_subst()`: Substitute command
  - `s_flag()`: Substitution flags processing
- **Features**: Advanced search with highlighting and confirmation

#### Undo System
- **Purpose**: Change tracking and undo/redo functionality
- **Key Functions**:
  - `log_append()`: Undo log append operations
  - `log_backward()`: Undo execution
  - `log_forward()`: Redo execution
  - `log_discard()`: Undo log cleanup
- **Features**: Comprehensive undo system with multiple levels

### MINIX-Specific Integration

#### Terminal Interface
- **Purpose**: MINIX terminal and console integration
- **Features**:
  - MINIX TTY service integration
  - Terminal capability detection
  - Screen size and capability management
  - Special key handling

#### File System Integration
- **Purpose**: MINIX VFS and file system operations
- **Features**:
  - VFS message-based file I/O
  - MINIX-specific file permissions
  - Recovery file management
  - Temporary file handling

#### Memory Management
- **Purpose**: MINIX VM integration for memory allocation
- **Features**:
  - Efficient memory allocation strategies
  - Large buffer management
  - Memory-mapped file support
  - Resource cleanup and management

## Editor State Architecture

### Editor State Structure
```c
struct _vi {
    SCR *scr;               /* Screen information */
    EXF *exf;               /* File information */
    VI_PRIVATE *private;    /* Private editor data */
    
    /* Current state */
    enum vi_mode mode;      /* Current editing mode */
    VICMD cmd;              /* Current command */
    
    /* Buffer management */
    TEXT_BUFFER *bp;        /* Current buffer pointer */
    size_t lineno;          /* Current line number */
    size_t column;          /* Current column */
    
    /* Search state */
    regex_t *search_re;     /* Compiled search pattern */
    char *search_pattern;   /* Search pattern string */
    
    /* Undo state */
    UNDO_LOG *undo_head;    /* Undo log head */
    UNDO_LOG *undo_current; /* Current undo position */
};
```

### Mode Management
- **Command Mode**: Normal command entry and execution
- **Insert Mode**: Text insertion and modification
- **Visual Mode**: Visual selection and operations
- **Ex Mode**: Command-line interface for complex operations
- **Replace Mode**: Character replacement mode

### Buffer System
- **Text Buffers**: Primary text storage and manipulation
- **Named Buffers**: Multiple named buffer support
- **Temporary Buffers**: Temporary text storage
- **File Buffers**: File-associated buffer management

## Build Configuration

### Makefile Structure (dist/common/)
```makefile
# Main vi program
PROG= vi
SRCS= vi.c ex.c v_screen.c v_scroll.c v_system.c \
      v_txt.c v_cmd.c v_put.c v_delete.c v_mark.c \
      v_motion.c v_search.c msg.c util.c mem.c \
      options.c options_f.c cut.c tmp.c log.c \
      main.c sex.c
MAN= vi.1 ex.1

# Include MINIX-specific configurations
.include "../Makefile.minix"
```

### Build Features
- **Cross-Platform**: Portable across different architectures
- **Feature Configuration**: Configurable feature set
- **Optimization**: Configurable optimization levels
- **Debug Support**: Optional debug output and tracing

## Integration with MINIX System

### Message-Based Architecture
Nvi integrates with MINIX through standard system interfaces:
- **VFS Integration**: File operations through virtual file system
- **TTY Integration**: Terminal control through TTY service
- **Memory Management**: Coordinates with VM for buffer management

### Service Dependencies
- **File System Services**: Heavy reliance on VFS for file operations
- **Terminal Services**: Uses TTY for screen and keyboard I/O
- **Memory Services**: Uses VM for dynamic buffer allocation

### MINIX-Specific Adaptations
- **Microkernel Optimization**: Efficient system call batching
- **Resource Management**: Careful resource allocation for embedded systems
- **Memory Efficiency**: Optimized for systems with limited memory
- **Error Recovery**: Enhanced crash recovery and file safety

## Advanced Features

### Multiple Window Support
- **Split Windows**: Horizontal and vertical window splitting
- **Window Navigation**: Efficient window switching and management
- **Independent Editing**: Independent cursor and buffer per window
- **Synchronized Scrolling**: Coordinated scrolling in related windows

### Extended Regular Expressions
- **Enhanced Syntax**: Extended regex features beyond POSIX
- **Backreferences**: Advanced pattern backreference support
- **Lookahead/Lookbehind**: Advanced regex assertions
- **Unicode Support**: Multi-byte character pattern matching

### Command Scripting
- **Macro Recording**: Keystroke recording and playback
- **Command Scripts**: Complex command sequences
- **Conditional Execution**: Conditional command execution
- **Loop Constructs**: Command repetition and iteration

### File Recovery System
- **Crash Recovery**: Automatic recovery from crashes
- **Backup Files**: Automatic backup file creation
- **Recovery Scripts**: Recovery script generation and execution
- **State Preservation**: Editor state preservation across sessions

## RISC-V 64-bit Considerations

### Architecture Optimization
- **Memory Alignment**: Proper alignment for RISC-V memory requirements
- **Large File Support**: Efficient 64-bit file addressing
- **Performance Tuning**: RISC-V-specific editor optimizations
- **Cross-Platform Compatibility**: Maintains consistent behavior

### Platform-Specific Features
- **Memory Efficiency**: Optimized for embedded RISC-V systems
- **Terminal Integration**: Optimized for MINIX TTY on RISC-V
- **Performance Characteristics**: Tuned for RISC-V pipeline architecture
- **Resource Constraints**: Careful resource usage for constrained environments

### Testing and Validation
- **RISC-V Specific Tests**: Architecture-specific editing validation
- **Performance Benchmarking**: Editor performance on RISC-V hardware
- **Memory Usage Analysis**: Memory consumption profiling on RISC-V
- **Compatibility Testing**: Cross-platform editing compatibility

## Usage Examples

### Basic Text Editing
```bash
# Open file for editing
vi filename.txt

# Insert mode
i    # Insert before cursor
a    # Insert after cursor
o    # Open new line below

# Basic navigation
h j k l    # Left, down, up, right
w b        # Word forward/backward
0 $        # Beginning/end of line

# Save and exit
:w    # Save
:q    # Quit
:wq   # Save and quit
:q!   # Force quit without saving
```

### Advanced Editing Operations
```bash
# Search and replace
:/pattern       # Search forward
:?pattern       # Search backward
:s/old/new/g    # Substitute on current line
:%s/old/new/g   # Substitute globally

# Copy and paste
yy    # Yank (copy) current line
p     # Paste after cursor
P     # Paste before cursor
"ayy  # Yank to register a
"ap   # Paste from register a

# Multiple operations
dd    # Delete current line
5dd   # Delete 5 lines
u     # Undo
Ctrl-r # Redo
```

### Window Management
```bash
# Split windows
:split filename    # Horizontal split
:vsplit filename   # Vertical split
Ctrl-w s           # Split current window
Ctrl-w v           # Vertical split

# Window navigation
Ctrl-w h/j/k/l     # Navigate between windows
Ctrl-w w           # Cycle through windows
Ctrl-w c           # Close current window
Ctrl-w o           # Close other windows
```

### Buffer Management
```bash
# File operations
:e filename        # Edit new file
:w filename        # Save as filename
:n                 # Next file
:p                 # Previous file
:ls                # List buffers
:b number          # Switch to buffer
```

### Search and Replace
```bash
# Advanced search
:/pattern/flags    # Search with flags
:%s/pat/repl/flags # Global substitution
:g/pat/cmd         # Global command
:v/pat/cmd         # Inverse global command

# Regular expressions
/^pattern          # Start of line
/pattern$          # End of line
/.*pattern.*/      # Contains pattern
/\<pattern\>        # Whole word
```

## Development and Configuration

### Configuration Files
- **System-wide**: `/etc/vi.exrc` - System default settings
- **User-specific**: `~/.exrc` - Personal configuration
- **Local**: `./.exrc` - Project-specific settings

### Common Configuration
```vim
" Line numbers
set number

" Syntax highlighting (if available)
syntax on

" Tab settings
set tabstop=4
set shiftwidth=4
set expandtab

" Search options
set ignorecase
set smartcase
set hlsearch

" Backup and recovery
set backup
set undofile
```

### Macro Definition
```vim
" Define macro in register q
qq                  # Start recording to register q
...commands...      # Execute commands
q                   # Stop recording

@q                  # Execute macro from register q
@@                  # Repeat last macro execution
```

## Performance Characteristics

### Scalability
- **Large Files**: Efficient editing of multi-megabyte files
- **Many Files**: Fast switching between multiple buffers
- **Complex Operations**: Optimized for complex editing sequences
- **Memory Efficiency**: Minimal memory usage for basic editing

### Optimization Features
- **Screen Update**: Optimized screen refresh algorithms
- **Buffer Management**: Efficient text buffer operations
- **Search Performance**: Fast pattern matching and search
- **Command Execution**: Optimized command processing

This nvi implementation provides a comprehensive, full-featured text editor with classic vi compatibility and modern extensions, optimized for the MINIX microkernel architecture and RISC-V 64-bit systems, forming the primary text editing tool for system administration and development work.