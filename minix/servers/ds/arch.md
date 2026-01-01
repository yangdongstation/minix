# Data Store Server Architecture

## Overview

The Data Store Server (DS) is a centralized key-value storage service in MINIX 3 that provides persistent and reliable storage for system-wide configuration data, state information, and inter-process communication data. It serves as a system-wide registry where servers and drivers can store and retrieve structured data.

## Purpose and Responsibilities

### Core Functions
- **Key-Value Storage**: Provides persistent storage for system data using string keys
- **Data Sharing**: Enables secure data sharing between system components
- **Configuration Management**: Stores system configuration and state information
- **Subscription Service**: Supports event notifications for data changes
- **Memory Management**: Efficient allocation and management of stored data
- **Access Control**: Implements ownership-based access control for stored entries

### Key Features
- Persistent storage across system restarts
- Regex-based subscription system for change notifications
- Memory-efficient storage with reference counting
- Owner-based access control and security
- Support for both static and dynamic data types
- Subscription-based change notifications

## Directory Structure

```
minix/servers/ds/
├── main.c              # Server main entry point and message handling
├── store.c             # Core storage implementation and data management
├── store.h             # Data structure definitions and constants
├── inc.h               # Include file with system headers
├── proto.h             # Function prototypes
└── Makefile            # Build configuration
```

## Core Components

### Storage Management (`store.c`)
Implements the core key-value storage functionality:
- Hash table-based key lookup
- Dynamic memory allocation for variable-length data
- Reference counting for memory management
- Garbage collection for unused entries
- Subscription management with regex matching

### Message Processing (`main.c`)
Handles incoming messages and coordinates operations:
- Message validation and routing
- Permission checking for operations
- Response generation and error handling
- System initialization and cleanup

## Data Structures

### Data Store Entry (`data_store`)
```c
struct data_store {
    int flags;                      /* Entry flags and properties */
    char key[DS_MAX_KEYLEN];        /* Lookup key */
    char owner[DS_MAX_KEYLEN];      /* Owner identification */
    union dsi_u {
        unsigned u32;               /* 32-bit integer data */
        struct dsi_mem {
            void *data;             /* Pointer to allocated data */
            size_t length;          /* Data length */
            size_t reallen;         /* Allocated buffer size */
        } mem;
    } u;
};
```

### Subscription Entry (`subscription`)
```c
struct subscription {
    int flags;                      /* Subscription properties */
    char owner[DS_MAX_KEYLEN];      /* Subscriber identification */
    regex_t regex;                  /* Pattern for matching keys */
    bitchunk_t old_subs[];          /* Bitmap of previous matches */
};
```

### Configuration Constants
- `NR_DS_KEYS`: Maximum number of stored entries (2 * NR_SYS_PROCS)
- `NR_DS_SUBS`: Maximum number of subscriptions (4 * NR_SYS_PROCS)
- `DS_MAX_KEYLEN`: Maximum length of storage keys

## Message Interface

### Key Operations
- `DS_PUBLISH`: Store or update a key-value pair
- `DS_RETRIEVE`: Fetch data by key
- `DS_DELETE`: Remove a key and its data
- `DS_SUBSCRIBE`: Register for change notifications
- `DS_CHECK`: Check if a key exists
- `DS_GET_LABEL`: Get ownership information

### Data Types
- **Integer Data**: 32-bit unsigned integers
- **Memory Data**: Variable-length binary data
- **String Data**: Null-terminated strings
- **Subscription Data**: Regex-based pattern matching

## Security and Access Control

### Ownership Model
- Each entry has an owner identified by a string label
- Only the owner can modify or delete their entries
- System components can read most entries
- Ownership prevents unauthorized modifications

### Subscription Security
- Subscribers only receive notifications for accessible keys
- Regex patterns are validated to prevent abuse
- Resource limits prevent denial-of-service attacks

## Subscription System

### Pattern Matching
- POSIX regex support for flexible key matching
- Bitmap tracking of previous matches
- Efficient notification delivery
- Wildcard and pattern-based subscriptions

### Notification Delivery
- Change notifications sent to subscribers
- Batch notification for multiple changes
- Error handling for failed deliveries
- Automatic cleanup of dead subscribers

## Memory Management

### Allocation Strategy
- Dynamic allocation for variable-length data
- Reference counting for shared entries
- Garbage collection for unused memory
- Memory pooling for frequently used sizes

### Optimization Techniques
- Hash table for O(1) key lookup
- Bitmap operations for subscription tracking
- Efficient regex compilation and caching
- Memory compaction during idle periods

## Integration with System Services

### PM (Process Manager)
- Process ownership tracking
- Cleanup on process termination
- Resource allocation coordination

### RS (Reincarnation Server)
- Service restart coordination
- Data persistence across restarts
- State recovery mechanisms

### VFS (Virtual File System)
- Configuration data for filesystems
- Mount information storage
- Device-specific parameters

## RISC-V Specific Considerations

### Memory Alignment
- RISC-V requires strict memory alignment
- Data structures optimized for RISC-V alignment requirements
- Efficient memory access patterns

### Atomic Operations
- Atomic operations for concurrent access
- Memory barriers for consistency
- RISC-V-specific optimizations

### Resource Constraints
- Embedded system considerations
- Efficient memory usage
- Scalable architecture for different platforms

## Performance Characteristics

### Lookup Performance
- Hash table provides O(1) average case lookup
- Optimized for frequent read operations
- Efficient subscription matching

### Memory Efficiency
- Minimal overhead per stored entry
- Efficient bitmap operations
- Memory pooling reduces fragmentation

### Scalability
- Configurable limits based on system size
- Efficient algorithms for large datasets
- Linear scaling with number of entries

## Error Handling

### Robustness Features
- Graceful handling of memory allocation failures
- Validation of all input parameters
- Atomic operations for consistency
- Recovery from corrupted data

### Error Conditions
- Key not found errors
- Permission denied errors
- Memory allocation failures
- Invalid parameter errors

## Testing and Debugging

### Debug Features
- Statistics and counters for operations
- Key enumeration for inspection
- Memory usage tracking
- Subscription monitoring

### Validation Tools
- Data integrity checking
- Memory leak detection
- Performance profiling
- Stress testing utilities

## Build Configuration

### Compilation Options
- Standard MINIX server build process
- Integration with system build system
- Architecture-specific optimizations
- Debug and release configurations

### System Integration
- Service registration in system.conf
- IPC permission configuration
- Startup dependencies
- Resource limits

## Use Cases and Examples

### Configuration Storage
- System-wide configuration parameters
- Driver-specific settings
- Network configuration
- Filesystem parameters

### State Information
- Service state tracking
- Runtime statistics
- Health monitoring data
- Performance metrics

### Inter-Process Communication
- Message queue management
- Shared state coordination
- Event notification
- Service discovery

## Future Enhancements

### Planned Features
- Encrypted storage for sensitive data
- Distributed data store for multi-node systems
- Advanced query capabilities
- Hierarchical key organization

### RISC-V Optimizations
- Platform-specific memory management
- Enhanced atomic operations
- Power-efficient storage algorithms
- Support for RISC-V cryptographic extensions

The Data Store Server provides a robust foundation for system-wide data management in MINIX 3, offering efficient storage, secure access control, and reliable operation across different architectures including RISC-V platforms.