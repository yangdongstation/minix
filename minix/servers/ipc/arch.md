# MINIX IPC Server Architecture

This directory contains the IPC (Inter-Process Communication) Server implementation, which provides System V style IPC mechanisms in MINIX. The IPC server manages shared memory, semaphores, and message queues, offering traditional UNIX IPC semantics while integrating with MINIX's message-based architecture.

## Core IPC Management Files

### Server Infrastructure
- **main.c**: Main IPC server implementation
  - Server initialization and service registration
  - Message loop and system call dispatch
  - Remote MIB (Management Information Base) integration
  - System V IPC compliance and compatibility
  - Error handling and logging
  - Server lifecycle management

- **inc.h**: IPC server include file
  - System header includes and dependencies
  - IPC-specific macro definitions
  - Function prototype declarations
  - Cross-module communication interfaces
  - NetBSD compatibility definitions

### Shared Memory Management
- **shm.c**: System V shared memory implementation
  - `do_shmget()`: Create or access shared memory segments
  - `do_shmat()`: Attach shared memory to process address space
  - `do_shmdt()`: Detach shared memory from process
  - `do_shmctl()`: Control shared memory operations
  - `get_shm_mib_info()`: Provide shared memory statistics
  - `is_shm_nil()`: Check if shared memory subsystem is active
  - `update_refcount_and_destroy()`: Reference counting and cleanup

**Shared Memory Features:**
- Segment creation with key-based identification
- Process attachment and detachment management
- Reference counting for automatic cleanup
- Permission-based access control
- Size validation and memory alignment
- Integration with VM server for memory mapping

### Semaphore Management
- **sem.c**: System V semaphore implementation
  - `do_semget()`: Create or access semaphore sets
  - `do_semctl()`: Control semaphore operations
  - `do_semop()`: Perform semaphore operations (P/V operations)
  - `get_sem_mib_info()`: Provide semaphore statistics
  - `is_sem_nil()`: Check if semaphore subsystem is active
  - `sem_process_event()`: Handle semaphore-related process events

**Semaphore Features:**
- Set-based semaphore management
- Atomic semaphore operations
- Process event handling for blocking operations
- Deadlock detection and prevention
- Permission-based access control
- Integration with PM server for process management

### Utility Functions
- **utility.c**: Common IPC utility functions
  - `check_perm()`: Validate IPC permissions
  - `prepare_mib_perm()`: Prepare MIB permission structures
  - Common permission checking logic
  - Security validation routines
  - Cross-platform compatibility functions

## System V IPC Implementation

### IPC Object Management
The IPC server maintains three types of IPC objects:

**Shared Memory Segments:**
- Unique key-based identification
- Size and permission attributes
- Attachment reference counting
- Automatic cleanup on last detachment
- Integration with virtual memory system

**Semaphore Sets:**
- Multiple semaphores per set
- Atomic operation support
- Process blocking and wakeup
- Deadlock prevention mechanisms
- Event-driven operation completion

**Message Queues:**
- FIFO message delivery
- Priority-based message ordering
- Size and count limitations
- Process synchronization
- Asynchronous operation support

### Permission Model
IPC objects use System V permission semantics:
- Owner, group, and world permissions
- Read, write, and control access modes
- Creator tracking and privilege inheritance
- Permission validation for all operations
- Integration with MINIX security model

## Message Passing Interface

### IPC System Calls
The IPC server handles standard System V IPC system calls:

**Shared Memory System Calls:**
- `IPC_SHMGET`: Create or access shared memory segment
- `IPC_SHMAT`: Attach segment to process address space
- `IPC_SHMDT`: Detach segment from process
- `IPC_SHMCTL`: Control shared memory operations

**Semaphore System Calls:**
- `IPC_SEMGET`: Create or access semaphore set
- `IPC_SEMCTL`: Control semaphore operations
- `IPC_SEMOP`: Perform semaphore P/V operations

### Call Vector Implementation
```c
static int (* const call_vec[])(message *) = {
    CALL(IPC_SHMGET)    = do_shmget,
    CALL(IPC_SHMAT)     = do_shmat,
    CALL(IPC_SHMDT)     = do_shmdt,
    CALL(IPC_SHMCTL)    = do_shmctl,
    CALL(IPC_SEMGET)    = do_semget,
    CALL(IPC_SEMCTL)    = do_semctl,
    CALL(IPC_SEMOP)     = do_semop,
};
```

### IPC Message Protocols
**Client Request Messages:**
- Synchronous request-response pattern
- Grant-based data transfer for large payloads
- Error code return via message status
- Support for both blocking and non-blocking operations

**Server Internal Messages:**
- Process event notifications from PM
- Memory management coordination with VM
- Statistics requests from MIB service
- Cleanup and garbage collection triggers

## Remote MIB Integration

### System Information Export
The IPC server integrates with MINIX's MIB (Management Information Base) system:

**kern.ipc.sysvipc_info Node:**
- `KERN_SYSVIPC_SEM_INFO`: Semaphore subsystem information
- `KERN_SYSVIPC_SHM_INFO`: Shared memory subsystem information
- `KERN_SYSVIPC_MSG`: Message queue support availability
- `KERN_SYSVIPC_SEM`: Semaphore support availability
- `KERN_SYSVIPC_SHM`: Shared memory support availability

### MIB Registration Process
1. Server initialization registers IPC subtree
2. MIB nodes are created for each IPC type
3. Statistics functions provide runtime information
4. Remote queries can access IPC subsystem state
5. Security restrictions apply to sensitive information

### Statistics and Monitoring
**Shared Memory Statistics:**
- Total number of active segments
- Total memory allocated for segments
- Attachment and detachment counts
- Creation and destruction statistics

**Semaphore Statistics:**
- Number of active semaphore sets
- Total semaphore operations performed
- Blocking and wakeup event counts
- Deadlock detection statistics

## Integration with MINIX System

### VM Server Coordination
Shared memory implementation coordinates with the VM server:
- Memory segment allocation and mapping
- Process address space management
- Reference counting for memory pages
- Cleanup on process termination
- Memory protection and access control

### PM Server Integration
Semaphore operations integrate with process management:
- Process blocking on semaphore operations
- Event notification for process state changes
- Cleanup of IPC resources on process exit
- Permission validation based on process credentials
- Integration with process scheduling

### VFS Server Interaction
IPC objects appear in the file system namespace:
- `/proc/sysvipc/shm`: Shared memory information
- `/proc/sysvipc/sem`: Semaphore information
- `/proc/sysvipc/msg`: Message queue information
- Integration with proc filesystem

## RISC-V Specific Implementation

### Memory Management
**Shared Memory on RISC-V:**
- Proper alignment for RISC-V memory requirements
- Support for RISC-V virtual memory architecture
- Integration with Sv39 page table management
- Atomic operations for reference counting
- Memory barrier usage for synchronization

**Semaphore Operations:**
- Atomic semaphore operations using RISC-V atomics
- Memory ordering constraints for RISC-V weak memory model
- Optimization for RISC-V cache hierarchy
- Efficient process wakeup mechanisms

### Performance Optimizations
- Efficient data structures for RISC-V cache characteristics
- Optimized memory allocation patterns
- Minimal overhead in critical IPC paths
- Scalable algorithms for large numbers of objects

## Configuration and Build

### Build Configuration
- **Makefile**: IPC server build configuration
  - Standard MINIX service build rules
  - Integration with system libraries
  - MIB service dependencies

### System Configuration
The IPC server is configured in `/etc/system.conf` with:
- IPC permissions for client communication
- System call privileges for memory management
- Integration with VM and PM servers
- MIB service registration requirements

### Configuration File
- **ipc.conf**: Server-specific configuration
  - IPC object limits and parameters
  - Memory allocation settings
  - Performance tuning options
  - Debug and logging configuration

## Development Guidelines

### Adding New IPC Mechanisms
1. Define new message types in system call table
2. Implement handler functions following existing patterns
3. Add permission checking and validation
4. Integrate with existing statistics and monitoring
5. Update MIB information export functions

### Memory Management Best Practices
- Proper reference counting for all objects
- Cleanup on all error paths
- Integration with VM server for memory operations
- Validation of memory parameters and sizes
- Prevention of memory leaks and corruption

### Synchronization Guidelines
- Use appropriate locking mechanisms
- Follow MINIX message passing conventions
- Handle race conditions in concurrent access
- Implement proper error recovery
- Test with multiple concurrent processes

### Security Considerations
- Validate all user-supplied parameters
- Implement proper permission checking
- Prevent resource exhaustion attacks
- Secure cleanup of sensitive data
- Audit trail for security-sensitive operations

## Fault Tolerance

### Crash Recovery
The IPC server supports MINIX's fault tolerance model:
- Automatic cleanup of orphaned resources
- Integration with RS for server restart
- Preservation of persistent IPC objects
- Recovery of shared memory mappings
- Restoration of semaphore state

### Resource Management
- Prevention of resource leaks
- Garbage collection of unused objects
- Limits on resource consumption
- Graceful handling of resource exhaustion
- Proper cleanup on process termination

This architecture provides comprehensive System V IPC support while maintaining MINIX's microkernel principles and fault tolerance requirements. The implementation balances compatibility with traditional UNIX IPC semantics and integration with MINIX's message-based architecture.