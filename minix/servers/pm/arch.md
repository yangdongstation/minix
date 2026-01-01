# MINIX Process Manager (PM) Architecture

This directory contains the Process Manager server implementation, which is one of the core system servers in MINIX. The PM handles all process-related operations including creation, termination, scheduling, signals, and process information management.

## Core Process Management Files

### Server Infrastructure
- **main.c**: PM server main program
  - Server initialization and setup
  - Message loop and dispatch
  - Service registration
  - Main server state management
  - Error handling and logging
  - Server lifecycle management

- **pm.h**: PM server definitions
  - PM-specific constants and macros
  - Server configuration parameters
  - PM service definitions
  - Internal data structure definitions

- **const.h**: PM constants
  - Process table size limits
  - Message type definitions
  - System call numbers
  - Return code definitions
  - Configuration constants

### Process Table Management
- **mproc.h**: Process table structure definitions
  - Process control block (PCB) structure
  - Process state information
  - Memory management information
  - Signal handling state
  - Scheduling parameters
  - Process identification data

- **glo.h**: Global variables
  - Process table array
  - Global PM state variables
  - System-wide counters
  - Configuration parameters
  - Shared state information

### Process Creation and Termination
- **forkexit.c**: Process creation and termination
  - fork() system call implementation
  - Process duplication logic
  - Memory space copying
  - File descriptor inheritance
  - Process ID allocation
  - Parent-child relationship setup
  - exit() system call implementation
  - Process cleanup procedures
  - Resource deallocation
  - Zombie process management
  - Orphaned process handling

- **exec.c**: Program execution
  - exec() family system calls
  - Program image loading
  - Argument and environment setup
  - Memory layout reorganization
  - File descriptor handling
  - Signal disposition reset
  - Process identity preservation

### Signal Handling
- **signal.c**: Signal management
  - Signal delivery mechanisms
  - Signal handler installation
  - Signal mask management
  - Signal queue processing
  - Real-time signal support
  - Signal stack management
  - Signal disposition tracking
  - Process group signaling
  - Session management

### Scheduling and Timing
- **schedule.c**: Process scheduling
  - Scheduling policy implementation
  - Priority management
  - Time quantum allocation
  - CPU time accounting
  - Scheduler activation
  - Process state transitions
  - Ready queue management

- **time.c**: Process timing
  - Process CPU time tracking
  - Alarm signal delivery
  - Interval timer support
  - Process accounting
  - Time statistics maintenance

### Context Management
- **mcontext.c**: Machine context operations
  - Process context switching
  - Register state management
  - Floating-point context
  - Signal context preservation
  - Context validation

### Alarm and Timer Management
- **alarm.c**: Alarm signal handling
  - Alarm() system call
  - Alarm signal delivery
  - Alarm queue management
  - Timer-based notifications
  - Process-specific alarms

### Event and State Management
- **event.c**: Event handling
  - Process event notification
  - State change events
  - Inter-process events
  - Event queue management
  - Asynchronous event processing

### System Information
- **getset.c**: Process information retrieval
  - getpid(), getppid() implementation
  - Process group information
  - Session information
  - User and group IDs
  - Process priority retrieval

### Utility Functions
- **misc.c**: Miscellaneous process operations
  - Process capability management
  - Resource limit handling
  - Process debugging support
  - Special process operations
  - Process statistics

- **profile.c**: Process profiling support
  - Performance monitoring
  - Process profiling data
  - Statistical collection
  - Profile analysis support

## Process Management Features

### Process Lifecycle
1. **Creation**: fork(), vfork(), clone() support
2. **Execution**: exec() family implementation
3. **Termination**: exit(), _exit(), abort() handling
4. **Cleanup**: Resource deallocation and parent notification
5. **Accounting**: Process statistics and resource usage

### Signal Architecture
- **Signal Delivery**: Reliable signal transmission
- **Signal Masking**: Per-process signal blocking
- **Signal Queuing**: Pending signal management
- **Signal Handlers**: User-defined signal processing
- **Real-time Signals**: POSIX real-time signal support
- **Signal Stacks**: Alternate signal stack support

### Scheduling Support
- **Priority Classes**: Multiple scheduling priorities
- **Time Quantum**: Fair CPU time allocation
- **CPU Affinity**: Process-to-CPU binding
- **Load Balancing**: Work distribution across CPUs
- **Real-time Support**: Real-time scheduling policies

## Integration with MINIX System

### Message-Based Interface
The PM communicates with other system components via message passing:
- **VFS Messages**: File system operations coordination
- **VM Messages**: Memory management collaboration
- **Kernel Messages**: Low-level system service requests
- **Driver Messages**: Device I/O coordination

### Inter-Server Communication
- **VFS Coordination**: File descriptor management
- **VM Collaboration**: Memory space operations
- **RS Interaction**: Service registration and monitoring
- **DM Cooperation**: Device access coordination

### Security Model
- **Permission Checking**: Process capability validation
- **Access Control**: Resource access authorization
- **Privilege Separation**: Minimal privilege principle
- **Audit Trail**: Process operation logging

## Error Handling

### Robust Error Management
- **Parameter Validation**: Input parameter checking
- **Resource Limits**: System limit enforcement
- **Error Propagation**: Consistent error reporting
- **Recovery Mechanisms**: Graceful failure handling
- **Debug Support**: Comprehensive debugging information

### Resource Management
- **Memory Allocation**: Dynamic memory management
- **Table Management**: Process table overflow handling
- **File Descriptor Tracking**: Open file management
- **Signal Queue Limits**: Pending signal limits
- **Process Limits**: Maximum process enforcement

## Performance Optimizations

### Efficient Algorithms
- **Fast Process Lookup**: Optimized process table access
- **Signal Delivery**: Efficient signal transmission
- **Context Switching**: Minimal overhead switching
- **Memory Copying**: Optimized process duplication

### Scalability Features
- **Multi-Core Support**: SMP scheduling algorithms
- **Load Distribution**: CPU load balancing
- **Memory Efficiency**: Minimal per-process overhead
- **Message Optimization**: Efficient IPC patterns

The Process Manager is a critical component of MINIX's microkernel architecture, providing robust, secure, and efficient process management while maintaining the system's fault tolerance and reliability principles.