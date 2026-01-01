# MINIX System Libraries Architecture

This directory contains MINIX-specific libraries that provide essential functionality for system components, servers, drivers, and applications. These libraries implement the MINIX-specific APIs and abstractions that enable the microkernel architecture.

## Core System Libraries

### System Call Interface (libsys/)
The primary system call library providing kernel interface:
- **alloc_util.c**: Memory allocation utilities
- **asynsend.c**: Asynchronous message sending
- **clock_time.c**: Clock and time functions
- **ds.c**: Data store interface
- **env_*.c**: Environment parsing and handling
- **getsysinfo.c**: System information retrieval
- **fkey_ctl.c**: Function key control
- **gcov.c**: Code coverage support
- **profile.c**: Performance profiling
- **arch/**: Architecture-specific system calls

### Driver Framework Libraries

#### Character Driver Framework (libchardriver/)
Framework for character device drivers:
- **chardriver.c**: Complete character driver implementation
- Provides standard interface for TTY, serial, and other character devices
- Handles message dispatching and common operations

#### Block Driver Framework (libblockdriver/)
Framework for block device drivers:
- **driver.c**: Single-threaded block driver
- **driver_mt.c**: Multi-threaded block driver
- **driver_st.c**: Single-threaded variant
- **drvlib.c**: Driver utility functions
- **mq.c/mq.h**: Message queue management
- **liveupdate.c**: Live update support
- **trace.c/trace.h**: Driver tracing

#### File System Driver (libfsdriver/)
Framework for file system implementations:
- Provides VFS interface abstractions
- Handles file system operations
- Manages file descriptors and paths

#### Network Driver (libnetdriver/)
Framework for network interface drivers:
- Provides network packet handling
- Manages network device registration
- Handles Ethernet frame processing

#### Socket Driver (libsockdriver/)
Framework for socket-based communication:
- Socket interface implementation
- Network protocol support

#### Input Driver (libinputdriver/)
Framework for input device drivers:
- Keyboard and mouse input handling
- Event processing and distribution

### Server Support Libraries

#### Device Manager Library (libdevman/)
Support for device manager operations:
- Device registration and enumeration
- Driver coordination
- Resource management

#### Timer Library (libtimers/)
Timer and timeout management:
- Timer registration and handling
- Timeout mechanisms
- Interval timing

#### Asynchronous Communication (libasyn/)
Asynchronous I/O support:
- Non-blocking I/O operations
- Event-driven programming
- Callback mechanisms

### File System Libraries

#### MINIX File System (libminixfs/)
MINIX native file system support:
- File system operations
- Metadata management
- Block allocation

#### Simplified File System (libsffs/)
Simple file system implementation:
- Basic file operations
- Minimal overhead design

#### Virtual Tree File System (libvtreefs/)
Virtual file system implementation:
- In-memory file systems
- Proc-like interfaces
- Dynamic content generation

### Architecture and Platform Support

#### Architecture Support (libacpi/)
ACPI (Advanced Configuration and Power Interface):
- Power management
- Hardware configuration
- System control

#### VirtIO Support (libvirtio/)
VirtIO device framework:
- Virtual device support
- Paravirtualized drivers
- Performance optimization

#### VirtIO MMIO (libvirtio_mmio/)
Memory-mapped VirtIO devices:
- Platform device support
- Memory-mapped I/O

### Communication Libraries

#### MINIX Threading (libmthread/)
Threading support for MINIX:
- POSIX thread implementation
- Thread synchronization
- Thread-local storage

#### Network Stack (liblwip/)
Lightweight IP stack:
- TCP/IP protocol implementation
- Network packet processing
- Socket interface

#### Socket Events (libsockevent/)
Socket event handling:
- Event notification
- Asynchronous socket operations
- Event multiplexing

### Audio and Multimedia

#### Audio Driver (libaudiodriver/)
Audio device support:
- Audio stream management
- Device abstraction
- Format conversion

### Platform Support

#### Clock Configuration (libclkconf/)
Clock and frequency management:
- System clock setup
- Frequency scaling
- Timing configuration

#### GPIO Support (libgpio/)
General Purpose I/O:
- Pin control and management
- Digital I/O operations
- Interrupt handling

#### I2C Driver (libi2cdriver/)
I2C bus support:
- Device communication
- Bus management
- Protocol implementation

#### USB Support (libusb/)
USB device framework:
- USB protocol handling
- Device enumeration
- Transfer management

### Virtualization Support

#### VirtualBox (libvboxfs/)
VirtualBox guest integration:
- Shared folders
- Guest additions
- Host-guest communication

#### VMware (libhgfs/)
VMware guest integration:
- Host-guest file system
- VMware tools support
- Virtual device management

### Debugging and Development

#### Assertion Library (libvassert/)
Assertion and debugging support:
- Runtime assertions
- Debug output
- Error checking

#### Magic Runtime (libmagicrt/)
Development and debugging tools:
- Runtime verification
- Magic number checking
- Memory debugging

#### GCC Support (libgcc_s_empty/)
GCC runtime support placeholder:
- Compiler support functions
- Runtime library interface

### Data Processing

#### MINIX Compression (libminc/)
Data compression support:
- Compression algorithms
- Archive handling
- Data optimization

#### Puffs (libpuffs/)
Pass-to-Userspace Framework:
- File system in userspace
- Protocol translation
- User-level file systems

## Library Design Principles

### Message-Based Interface
All libraries follow MINIX's message-passing paradigm:
- Synchronous request-response for most operations
- Asynchronous notifications for events
- Structured message formats
- Type-safe interfaces

### Error Handling
Consistent error handling across libraries:
- Standardized error codes
- Meaningful error messages
- Graceful degradation
- Recovery mechanisms

### Resource Management
Proper resource lifecycle management:
- Allocation and deallocation tracking
- Resource cleanup on failure
- Memory leak prevention
- Efficient resource usage

### Thread Safety
Libraries support concurrent access:
- Thread-safe operations where applicable
- Proper synchronization primitives
- Race condition avoidance
- Deadlock prevention

## Build Integration

### Makefile Structure
- **Makefile**: Top-level library build configuration
- **Makefile.inc**: Common build definitions
- Individual library Makefiles for specific builds

### Dependencies
Libraries are built in dependency order:
1. Core system libraries (libsys)
2. Framework libraries (libchardriver, libblockdriver)
3. Specialized libraries (network, file system)
4. Application libraries (user-level functionality)

This comprehensive library collection provides the foundation for MINIX's modular architecture, enabling efficient development of system components while maintaining the microkernel design philosophy.