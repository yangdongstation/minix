# MINIX Management Information Base (MIB) Server Architecture

This directory contains the Management Information Base (MIB) Server implementation, which provides the sysctl(2) system call interface for MINIX. The MIB server implements a hierarchical tree structure for system information access, allowing both local and remote management of kernel parameters, hardware information, and system statistics.

## Core MIB Management Files

### Server Infrastructure
- **main.c**: Main MIB server implementation
  - Server initialization and sysctl tree setup
  - Message loop and system call dispatch
  - Request abstraction and first-level tree processing
  - Remote subtree registration and management
  - User data handling and validation
  - Error handling and logging
  - Superuser privilege management

- **mib.h**: MIB server definitions and structures
  - MIB node structure definitions
  - Node flag definitions and interpretations
  - Call parameter structures
  - Tree management macros and utilities
  - Remote subtree registration interfaces
  - Platform-specific configuration options

### System Information Trees
- **kern.c**: Kernel information subtree (CTL_KERN)
  - Process information and statistics
  - System architecture and hardware details
  - Boot parameters and configuration
  - Security and privilege information
  - Time and scheduling parameters
  - File system and I/O statistics
  - IPC subsystem information
  - Signal and exception handling

- **vm.c**: Virtual memory subtree (CTL_VM)
  - Memory usage statistics
  - Virtual memory parameters
  - Page allocation information
  - Swap space management
  - Memory pressure indicators
  - VM subsystem configuration

- **hw.c**: Hardware information subtree (CTL_HW)
  - CPU architecture information
  - Hardware configuration details
  - Device and peripheral information
  - Memory layout and organization
  - Platform-specific hardware data

- **proc.c**: Process information subtree
  - Per-process statistics and information
  - Process tree and hierarchy data
  - Resource usage tracking
  - Process state information
  - Debugging and tracing data

- **minix.c**: MINIX-specific subtree (CTL_MINIX)
  - MINIX-specific kernel parameters
  - Microkernel configuration
  - Service and server information
  - MINIX-specific statistics
  - Boot and initialization data

### Remote Subtree Management
- **remote.c**: Remote MIB subtree handling
  - Remote service registration
  - Subtree mounting and unmounting
  - Request forwarding to remote services
  - Response aggregation and validation
  - Remote service lifecycle management
  - Blocking and non-blocking remote calls

- **tree.c**: MIB tree management and traversal
  - Tree node allocation and deallocation
  - Tree traversal and search algorithms
  - Node type handling and validation
  - Data retrieval and modification
  - Permission checking and validation
  - Tree consistency maintenance

## MIB Tree Architecture

### Hierarchical Structure
The MIB implements a hierarchical tree structure based on BSD sysctl conventions:

**Top-Level Nodes:**
- **CTL_KERN**: Kernel parameters and information
- **CTL_VM**: Virtual memory subsystem
- **CTL_NET**: Networking parameters (remote registration)
- **CTL_HW**: Hardware information
- **CTL_USER**: User-level parameters (libc handled)
- **CTL_VENDOR**: Vendor-specific parameters (writable)
- **CTL_MINIX**: MINIX-specific parameters

### Node Types and Properties
**Node Structure:**
```c
struct mib_node {
    unsigned int node_flags;    /* Node type and properties */
    unsigned int node_size;     /* Data size for data nodes */
    unsigned int node_version;  /* Version tracking */
    const char *node_name;      /* Node name */
    const char *node_desc;      /* Description */
    /* Union of type-specific data */
};
```

**Node Flags:**
- **CTLFLAG_PARENT**: Node is a real parent node
- **CTLFLAG_VERIFY**: Node has verification function
- **CTLFLAG_REMOTE**: Node is root of remote subtree
- **CTLFLAG_IMMEDIATE**: Data is stored in node structure
- **CTLFLAG_READONLY/CTLFLAG_READWRITE**: Access permissions

### Data Node Types
**Supported Data Types:**
- **CTLTYPE_BOOL**: Boolean values
- **CTLTYPE_INT**: Integer values
- **CTLTYPE_QUAD**: 64-bit integer values
- **CTLTYPE_STRING**: String values
- **CTLTYPE_STRUCT**: Complex structures
- **CTLTYPE_NODE**: Directory nodes

## Message Passing Interface

### sysctl(2) System Call Implementation
The MIB server implements the complete sysctl(2) system call interface:

**System Call Parameters:**
- Name array specifying tree path
- Name length indicating path depth
- Old data buffer for retrieval
- Old size pointer for buffer sizing
- New data buffer for modification
- New size for modification data

### Request Processing Flow
1. **Path Resolution**: Traverse tree based on name array
2. **Permission Checking**: Validate access permissions
3. **Handler Invocation**: Call appropriate handler function
4. **Data Processing**: Retrieve or modify data as requested
5. **Response Generation**: Return results to user process

### Handler Function Types
**Local Handlers:**
- Direct data access functions
- Computed value generators
- Static data providers
- Complex structure handlers

**Remote Handlers:**
- Request forwarding functions
- Remote service coordination
- Response aggregation
- Error propagation

## Remote Subtree Registration

### Registration Process
Services can register remote subtrees to extend the MIB tree:

**Registration Steps:**
1. Service prepares subtree definition
2. Calls `rmib_register()` with path and node
3. MIB server validates registration request
4. Remote subtree is mounted at specified path
5. Requests to subtree are forwarded to service

### Remote Call Mechanism
**Request Forwarding:**
- MIB server receives sysctl request
- Determines if path leads to remote subtree
- Forwards request to registered service
- Waits for response (blocking call)
- Returns response to original caller

**Remote Handler Interface:**
```c
ssize_t remote_handler(struct rmib_call *call,
                      struct rmib_node *node,
                      struct rmib_oldp *oldp,
                      struct rmib_newp *newp);
```

### Practical Restrictions
Remote subtrees have additional limitations:
- Blocking calls to remote services
- Limited error recovery mechanisms
- Registration persistence requirements
- Service lifecycle dependencies

## Integration with MINIX System

### Service Dependencies
The MIB server coordinates with multiple system services:

**Information Sources:**
- **Kernel**: Core system parameters and statistics
- **PM**: Process information and management data
- **VM**: Virtual memory statistics and parameters
- **VFS**: File system information and statistics
- **IPC**: System V IPC information
- **RS**: Service and server information

**Information Consumers:**
- **User processes**: System information queries
- **Init**: Boot-time parameter access
- **ProcFS**: Process filesystem implementation
- **System utilities**: Configuration and monitoring tools

### Privilege Management
The MIB server requires superuser privileges:
- Access to privileged kernel information
- Modification of system parameters
- Registration of remote subtrees
- Coordination with system services

### Boot-Time Requirements
The MIB server is included in the boot image because:
- Init(8) uses sysctl(2) during startup
- Early system configuration access
- Critical for system initialization
- Required for service startup sequence

## RISC-V Specific Implementation

### Platform Information
**Hardware Information (CTL_HW):**
- CPU architecture detection for RISC-V
- Machine type and platform identification
- Memory layout and organization
- Device tree integration
- Platform-specific parameters

**Kernel Information (CTL_KERN):**
- RISC-V specific boot parameters
- Hardware capability detection
- Platform feature identification
- CPU topology and configuration

### Memory Management
**Virtual Memory (CTL_VM):**
- RISC-V virtual memory architecture support
- Sv39 page table information
- Memory management unit statistics
- TLB and cache information

### Performance Optimizations
- Efficient tree traversal algorithms
- Optimized data structure layouts
- Minimal overhead in critical paths
- Scalable architecture support

## Configuration and Build

### Build Configuration
- **Makefile**: MIB server build configuration
  - Standard MINIX service build rules
  - Integration with system libraries
  - Architecture-specific optimizations

### System Configuration
The MIB server is configured in system configuration with:
- Superuser privileges for system access
- IPC permissions for service communication
- Boot image inclusion for early availability
- Integration with system initialization

### Development Configuration
- **MINIX_TEST_SUBTREE**: Include test subtree for development
- **MIB_DEBUG_MOUNT**: Debug output for remote registration
- Conditional compilation for production vs. development

## Development Guidelines

### Adding New MIB Nodes
1. Define node structure and handler function
2. Add node to appropriate subtree table
3. Implement data access or computation logic
4. Add permission checking if required
5. Update documentation and descriptions

### Remote Subtree Development
1. Implement remote handler functions
2. Define subtree structure and nodes
3. Register subtree during service initialization
4. Handle request forwarding and responses
5. Implement proper error handling

### Security Considerations
- Validate all user-supplied parameters
- Implement proper permission checking
- Prevent information disclosure
- Secure handling of sensitive data
- Audit trail for security-sensitive operations

### Performance Guidelines
- Minimize tree traversal overhead
- Optimize frequently accessed nodes
- Use efficient data structures
- Implement caching where appropriate
- Profile and optimize critical paths

## Fault Tolerance

### Error Handling
The MIB server implements comprehensive error handling:
- Invalid path resolution
- Permission denied conditions
- Handler function failures
- Remote service unavailability
- Memory allocation failures

### Recovery Mechanisms
- Graceful degradation on service failures
- Automatic cleanup on errors
- Resource management and limits
- Service restart coordination
- State consistency maintenance

### Resource Management
- Memory allocation tracking
- Remote service reference counting
- Tree node lifecycle management
- Request timeout handling
- Cleanup on service termination

This architecture provides a comprehensive and extensible system information management framework that supports both traditional BSD sysctl functionality and MINIX-specific requirements while maintaining system reliability and performance.