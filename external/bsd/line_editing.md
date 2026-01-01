# MINIX 3 BSD Line Editing Architecture

## Overview

The BSD line editing capabilities in MINIX 3 are provided through integrated components within various BSD-licensed utilities rather than a standalone libedit package. This architecture document describes the comprehensive command line editing functionality embedded throughout the MINIX system, including terminal handling, shell integration, and the sophisticated interactive text input mechanisms that provide Unix-compatible line editing across all BSD-derived components.

## Line Editing Architecture Context

### BSD Line Editing Philosophy
- **Integrated Approach**: Line editing functionality embedded within utilities
- **Terminal-Centric**: Deep integration with terminal handling and TTY subsystem
- **Consistency**: Uniform line editing behavior across BSD utilities
- **Resource Efficiency**: Minimal overhead for embedded and resource-constrained systems
- **Historical Compatibility**: Preservation of traditional Unix line editing behavior

### Implementation Strategy
- **Utility Integration**: Line editing built into shells and interactive utilities
- **Shared Components**: Common line editing code shared across utilities
- **Terminal Database**: Integration with termcap/terminfo for terminal capabilities
- **Signal Coordination**: Proper coordination with signal handling and job control
- **Memory Efficiency**: Optimized for minimal memory usage

## Core Line Editing Components

### Terminal Line Discipline Integration

#### BSD Terminal Handling Architecture
```c
// BSD terminal line discipline integration
#include <termios.h>
#include <unistd.h>

// Terminal configuration for line editing
struct termios original_term, raw_term;

// Configure raw mode for line editing
void setup_line_editing(int fd) {
    tcgetattr(fd, &original_term);
    raw_term = original_term;
    
    // Configure for line editing
    raw_term.c_lflag &= ~(ICANON | ECHO);
    raw_term.c_cc[VMIN] = 1;
    raw_term.c_cc[VTIME] = 0;
    
    // Set line editing characters
    raw_term.c_cc[VERASE] = '\b';     // Backspace
    raw_term.c_cc[VKILL] = '\x15';    // Ctrl-U (kill line)
    raw_term.c_cc[VWERASE] = '\x17';  // Ctrl-W (word erase)
    raw_term.c_cc[VLNEXT] = '\x16';   // Ctrl-V (literal next)
    raw_term.c_cc[VEOF] = '\x04';     // Ctrl-D (EOF)
    raw_term.c_cc[VINTR] = '\x03';    // Ctrl-C (interrupt)
    raw_term.c_cc[VQUIT] = '\x1c';    // Ctrl-\ (quit)
    raw_term.c_cc[VSUSP] = '\x1a';    // Ctrl-Z (suspend)
    
    tcsetattr(fd, TCSANOW, &raw_term);
}
```

#### RISC-V 64-bit Terminal Support
- **Character Processing**: RISC-V-optimized character input processing
- **Signal Handling**: Efficient signal delivery and handling
- **Memory Management**: Optimized buffer management for 64-bit systems
- **I/O Performance**: High-performance terminal I/O operations

### Shell Command Line Editing Integration

#### Enhanced Shell Capabilities
```bash
# MINIX shell line editing configuration
set -o emacs          # Enable Emacs-style line editing
set -o vi             # Alternative: Vi-style line editing
set -o histexpand     # Enable history expansion
set -o histreedit     # Allow re-editing failed history expansions

# History configuration
HISTSIZE=1000         # Maximum history entries in memory
HISTFILESIZE=2000     # Maximum history entries in file
HISTFILE=~/.sh_history # History file location
HISTCONTROL=ignoredups # Ignore duplicate consecutive commands
HISTIGNORE="ls:cd:pwd" # Ignore specific commands in history
```

#### Advanced Shell Line Editing Features
- **Command History**: Comprehensive command history with persistent storage
- **History Expansion**: Csh-style history expansion (!$, !!, !n, etc.)
- **Incremental Search**: Ctrl-R for reverse incremental search
- **Word Navigation**: Alt-B/Alt-F for word-by-word navigation
- **Line Manipulation**: Ctrl-A/Ctrl-E for beginning/end of line

### Utility-Specific Line Editing Implementation

#### BSD Utility Integration Pattern
```c
// Common line editing structure for BSD utilities
struct line_edit_state {
    char *buffer;           // Input buffer
    size_t buffer_size;     // Buffer size
    size_t cursor_pos;      // Current cursor position
    size_t line_length;     // Current line length
    struct history_state *history; // Command history
    int terminal_fd;        // Terminal file descriptor
    struct termios original_term;  // Original terminal settings
};

// Line editing main loop
char *line_edit_readline(struct line_edit_state *state, const char *prompt) {
    write(state->terminal_fd, prompt, strlen(prompt));
    
    while (1) {
        char ch;
        if (read(state->terminal_fd, &ch, 1) != 1) break;
        
        switch (ch) {
            case '\b':  // Backspace
            case 127:   // Delete
                line_edit_delete_char(state);
                break;
            case '\x15': // Ctrl-U (kill line)
                line_edit_kill_line(state);
                break;
            case '\x17': // Ctrl-W (word erase)
                line_edit_delete_word(state);
                break;
            case '\n':   // Enter
            case '\r':   // Return
                return line_edit_finish(state);
            default:
                line_edit_insert_char(state, ch);
        }
    }
    return NULL;
}
```

#### Key Binding Architecture
```c
// Key binding structure for line editing
struct key_binding {
    int key;                    // Key code
    const char *sequence;       // Escape sequence (if applicable)
    void (*handler)(void *);    // Handler function
    const char *description;    // Description for help
};

// Emacs-style key bindings
struct key_binding emacs_bindings[] = {
    { CTRL('A'), NULL, beginning_of_line, "Move to beginning of line" },
    { CTRL('E'), NULL, end_of_line, "Move to end of line" },
    { CTRL('B'), NULL, backward_char, "Move backward one character" },
    { CTRL('F'), NULL, forward_char, "Move forward one character" },
    { CTRL('D'), NULL, delete_char, "Delete character under cursor" },
    { CTRL('K'), NULL, kill_line, "Kill to end of line" },
    { CTRL('U'), NULL, unix_line_discard, "Kill entire line" },
    { CTRL('W'), NULL, unix_word_rubout, "Kill previous word" },
    { CTRL('R'), NULL, reverse_search_history, "Reverse search history" },
    { 0, NULL, NULL, NULL }
};
```

## Advanced Line Editing Features

### Command History Management

#### History Architecture
```c
// Command history management system
struct history_entry {
    char *command;          // Command text
    time_t timestamp;       // Execution timestamp
    int flags;              // Entry flags (duplicates, etc.)
};

struct history_state {
    struct history_entry *entries;  // History entries array
    int count;                      // Number of entries
    int max_size;                   // Maximum entries
    int current_index;              // Current position for navigation
    char *history_file;             // Persistent storage file
    int flags;                      // History behavior flags
};

// History file format for persistence
#define HISTORY_MAGIC 0x48535454    // "HSTT"
#define HISTORY_VERSION 1

struct history_file_header {
    uint32_t magic;                 // Magic number
    uint32_t version;               // File format version
    uint32_t entry_count;           // Number of entries
    uint32_t flags;                 // Global flags
    time_t timestamp;               // File creation time
};
```

#### Advanced History Features
- **Duplicate Suppression**: Intelligent suppression of duplicate commands
- **Timestamp Recording**: Command execution timestamp recording
- **Search Capabilities**: Forward and reverse history search
- **Persistent Storage**: Automatic persistence across sessions
- **Size Management**: Configurable history size limits

### Multi-Modal Editing Support

#### Emacs-Style Editing
- **Basic Navigation**: Ctrl-A, Ctrl-E, Ctrl-B, Ctrl-F for cursor movement
- **Text Deletion**: Ctrl-D, Ctrl-K, Ctrl-W for various deletion operations
- **History Navigation**: Ctrl-P, Ctrl-N for previous/next in history
- **Search Functions**: Ctrl-R for reverse incremental search

#### Vi-Style Editing (when enabled)
- **Modal Operation**: Command mode and insert mode
- **Command Navigation**: h, l, w, b for character and word movement
- **Text Operations**: x, d, y for delete, yank operations
- **Mode Switching**: i, a, ESC for mode transitions

### Terminal Capability Integration

#### Termcap/Terminfo Integration
```c
// Terminal capability detection and usage
#include <term.h>
#include <curses.h>

// Initialize terminal capabilities
int setup_terminal_capabilities(void) {
    char *term = getenv("TERM");
    if (term == NULL) term = "vt100";
    
    if (tgetent(NULL, term) != 1) {
        return -1;  // Terminal database not available
    }
    
    // Query specific capabilities
    const char *clear_line = tgetstr("ce", NULL);  // Clear to end of line
    const char *cursor_left = tgetstr("le", NULL); // Cursor left
    const char *cursor_right = tgetstr("nd", NULL); // Cursor right
    const char *delete_char = tgetstr("dc", NULL); // Delete character
    
    return 0;
}
```

#### Advanced Terminal Features
- **Cursor Control**: Precise cursor positioning and control
- **Screen Management**: Efficient screen update mechanisms
- **Color Support**: Terminal color support when available
- **Special Keys**: Support for special keys (Home, End, arrows, function keys)

## Build System Integration

### Cross-Compilation Support
```makefile
# Line editing functionality cross-compilation
HOST_CC=gcc
TARGET_CC=riscv64-unknown-elf-gcc

# Terminal and line editing libraries
LIB=linedit
SRCS= line_edit.c history.c key_binding.c terminal.c
CFLAGS=-O2 -DLINE_EDITING -DEMACS_MODE -DVI_MODE -DRISCV_64
CPPFLAGS=-I${MINIX_ROOT}/include -I${MINIX_ROOT}/include/bsd
LDFLAGS=-static

# Integration with utilities
PROG= utility_with_line_edit
SRCS+= utility.c
LDADD+= -llinedit -ltermlib
.include <bsd.prog.mk>
```

### MINIX Integration Configuration
- **TTY Integration**: Integration with MINIX TTY subsystem
- **Signal Handling**: Proper integration with MINIX signal handling
- **Process Management**: Coordination with MINIX process management
- **Memory Management**: Integration with MINIX memory management

## RISC-V 64-bit Architecture Support

### Memory Architecture Integration
- **Large Buffer Support**: Support for large input buffers using 64-bit addressing
- **Memory Mapping**: Efficient memory mapping for terminal operations
- **Cache Optimization**: Cache-friendly data structures for line editing
- **Atomic Operations**: RISC-V atomic operations for thread-safe operations

### Performance Optimization
- **64-bit Register Usage**: Optimal use of RISC-V 64-bit registers
- **Instruction Selection**: RISC-V-optimized instruction sequences
- **Memory Access**: Optimized memory access patterns
- **I/O Efficiency**: High-efficiency I/O operations for RISC-V hardware

### Embedded System Considerations
- **Memory Efficiency**: Minimal memory footprint for embedded systems
- **Real-time Performance**: Predictable performance for real-time applications
- **Resource Constraints**: Optimization for resource-constrained environments
- **Power Efficiency**: Power-efficient operation for battery-powered devices

## Integration with MINIX System Architecture

### Terminal Subsystem Integration
```c
// MINIX terminal subsystem integration
#include <minix/tty.h>
#include <minix/ipc.h>

// Integration with MINIX TTY driver
int minix_terminal_line_edit(int tty_line, char *buffer, size_t size) {
    message msg;
    
    // Request line editing service from TTY driver
    msg.m_type = TTY_LINE_EDIT;
    msg.TTY_LINE = tty_line;
    msg.TTY_BUFFER = buffer;
    msg.TTY_SIZE = size;
    
    return sendrec(TTY_PROC_NR, &msg);
}
```

### Process and Signal Management
- **Job Control Integration**: Integration with MINIX job control
- **Signal Coordination**: Proper coordination with signal delivery
- **Process Groups**: Support for process group line editing
- **Session Management**: Integration with session management

### File System and Persistence
- **History File Management**: Integration with MINIX file system for history
- **Configuration Storage**: Support for line editing configuration files
- **Temporary File Handling**: Proper temporary file management
- **Permission Handling**: Correct file permission and security handling

## Advanced Features and Capabilities

### Unicode and Internationalization
- **UTF-8 Support**: Full support for UTF-8 character input
- **Wide Character**: Wide character handling for international input
- **Locale Integration**: Integration with MINIX locale system
- **Input Method Support**: Support for input method frameworks

### Accessibility and Usability
- **Screen Reader Support**: Compatibility with screen reader software
- **High Contrast Support**: Support for high contrast displays
- **Keyboard Navigation**: Enhanced keyboard navigation capabilities
- **Customizable Interface**: Fully customizable key binding interface

### Network and Remote Access
- **SSH Integration**: Full integration with SSH for remote sessions
- **Serial Console**: Support for serial console line editing
- **Network Transparency**: Network-transparent line editing capabilities
- **Latency Handling**: Proper handling of network latency

## Development and Quality Assurance

### Historical Compatibility
- **BSD Compatibility**: Maintenance of BSD compatibility
- **Traditional Behavior**: Preservation of traditional Unix line editing
- **Interface Stability**: Stable interfaces for backward compatibility
- **Documentation**: Comprehensive documentation of features

### Modern Enhancements
- **Performance Optimization**: Modern performance optimizations
- **Security Improvements**: Security enhancements while maintaining compatibility
- **Unicode Support**: Modern Unicode and internationalization support
- **Accessibility Features**: Enhanced accessibility features

### Quality Assurance Framework
- **Compatibility Testing**: Testing compatibility with traditional systems
- **Performance Testing**: Performance testing and optimization
- **Integration Testing**: Integration testing with MINIX components
- **Regression Testing**: Prevention of regressions in functionality

This BSD line editing architecture provides comprehensive interactive text input capabilities throughout the MINIX system while maintaining BSD licensing compatibility and optimized performance for the RISC-V 64-bit architecture. The integration focuses on preserving traditional Unix line editing functionality while providing modern enhancements for contemporary interactive computing needs.