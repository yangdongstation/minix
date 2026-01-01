# MINIX System Servers Architecture

This directory contains the core system servers that implement operating system functionality in userspace. These servers run as separate processes and communicate with each other and the kernel through message passing, following MINIX's microkernel philosophy.

## Core System Servers

### Process Manager (pm/)
The Process Manager handles all process-related operations:
- **main.c**: Server initialization and message loop
- **forkexit.c**: Process creation (fork) and termination (exit)
- **exec.c**: Program execution (exec family of system calls)
- **signal.c**: Signal handling and delivery
- **alarm.c**: Alarm and timer functionality
- **schedule.c**: Process scheduling policies
- **misc.c**: Various process control operations
- **event.c**: Event handling for processes
- **time.c**: Process timing and statistics
- **trace.c**: Process tracing and debugging support
- **getset.c**: Getting and setting process parameters
- **mcontext.c**: Machine context handling
- **mproc.h**: Process table structure definitions
- **pm.h**: Process manager specific definitions

### Virtual File System (vfs/)
The VFS server provides a unified interface to all file systems:
- **main.c**: VFS server initialization and main loop
- **filedes.c**: File descriptor management
- **open.c**: File opening and creation
- **read.c/write.c**: File I/O operations
- **link.c**: File linking operations
- **exec.c**: File execution support
- **cdev.c**: Character device handling
- **bdev.c**: Block device handling
- **pipe.c**: Pipe file system implementation
- **select.c**: Select/poll functionality
- **lock.c**: File locking mechanisms
- **mount.c**: File system mounting
- **path.c**: Path resolution
- **protect.c**: File permission handling
- **stat.c**: File status operations
- **dmap.c/dmap.h**: Device mapping table
- **fproc.h**: File process table structure
- **fs.h**: VFS server definitions

### Virtual Memory Manager (vm/)
The VM server handles memory management:
- **main.c**: VM server initialization
- **alloc.c**: Memory allocation
- **fork.c**: Memory handling for fork operations
- **exec.c**: Memory setup for program execution
- **exit.c**: Memory cleanup on process exit
- **signal.c**: Signal stack management
- **utility.c**: Memory utility functions
- **mcontext.c**: Memory context operations
- **vm.h**: VM server definitions
- **vmproc.h**: VM process table structure

### Reincarnation Server (rs/)
The RS monitors and restarts crashed servers:
- **main.c**: RS server initialization
- **server.c**: Server management
- **exec.c**: Server execution
- **restart.c**: Server restart logic
- **sysctl.c**: System control operations
- **rs.h**: RS server definitions

### Device Manager (devman/)
The Device Manager coordinates hardware device access:
- **main.c**: Device manager initialization
- **devman.c**: Core device management
- **driver.c**: Driver coordination
- **device.c**: Device abstraction
- **devman.h**: Device manager definitions

## Supporting Servers

### Information Server (is/)
Provides system information services:
- System statistics and information
- Process information
- Resource usage data

### Data Store (ds/)
Manages persistent system data:
- System configuration storage
- Persistent data management
- Key-value data store

### Scheduling Server (sched/)
Handles advanced scheduling policies:
- Process scheduling algorithms
- Resource allocation policies
- Performance optimization

### Message Interception Server (ipc/)
Manages IPC filtering and monitoring:
- Message filtering
- IPC debugging
- Communication monitoring

### Management Information Base (mib/)
Provides SNMP-like management information:
- System monitoring data
- Performance metrics
- Network management information

### Input Server (input/)
Handles input device coordination:
- Keyboard input management
- Mouse input handling
- Input event distribution

## Server Communication Architecture

### Message Types
Each server handles specific message types:
- **PM**: Process control messages (fork, exit, signal, etc.)
- **VFS**: File system messages (open, read, write, etc.)
- **VM**: Memory management messages (alloc, free, map, etc.)
- **RS**: Server management messages (start, stop, restart, etc.)
- **DM**: Device management messages (register, query, control, etc.)

### IPC Pattern
Servers communicate using:
- **Synchronous IPC**: Request-response pattern for most operations
- **Asynchronous IPC**: Event notifications and status updates
- **Notifications**: Non-blocking status and event delivery

### Privilege Model
Each server has specific privileges:
- **IPC Permissions**: Which servers they can communicate with
- **System Call Access**: Which kernel services they can use
- **I/O Access**: Which hardware resources they can control
- **Memory Access**: What memory regions they can access

## Configuration Files

### System Configuration
- **Makefile**: Top-level server build configuration
- **Makefile.inc**: Common build definitions

### Service Configuration
System services are configured in `/etc/system.conf` with:
- IPC target permissions
- System call privileges
- I/O port ranges
- IRQ assignments
- Memory access rights

## Fault Tolerance

### Crash Recovery
The Reincarnation Server provides fault tolerance by:
1. Monitoring all system servers
2. Detecting server crashes
3. Automatically restarting failed servers
4. Maintaining system availability

### Server Isolation
Each server runs in isolation:
- Separate address space
- Limited privileges
- Controlled communication channels
- Independent failure domains

## Development Guidelines

### Adding New Servers
To add a new system server:
1. Implement the server following the message-passing pattern
2. Define server-specific message types
3. Register with the Reincarnation Server
4. Configure IPC permissions in system.conf
5. Add build rules to the Makefile

### Server Design Principles
- Keep servers focused on specific functionality
- Use message passing for all communication
- Handle errors gracefully
- Support restart and recovery
- Minimize dependencies between servers

This architecture enables MINIX 3's high reliability by isolating system functionality into independent, restartable components that communicate through well-defined interfaces.