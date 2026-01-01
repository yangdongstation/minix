# Device Manager Server Architecture

## Overview

The Device Manager (devman) is a critical system server in MINIX 3 responsible for managing device registration, discovery, and providing a unified interface for device information access. It acts as a central registry for all system devices and enables dynamic device management in the microkernel environment.

## Purpose and Responsibilities

### Core Functions
- **Device Registration**: Manages device registration from drivers and system components
- **Device Discovery**: Provides mechanisms for discovering available devices
- **Device Information**: Maintains and serves device metadata and properties
- **Device Hierarchy**: Manages parent-child relationships between devices
- **Device Events**: Handles device addition/removal events and notifications
- **Virtual Filesystem**: Provides device information through a VFS interface

### Key Features
- Dynamic device management without system restart
- Hierarchical device tree organization
- Event-driven notifications for device changes
- Virtual filesystem interface for device information access
- Reference counting for device lifecycle management

## Directory Structure

```
minix/servers/devman/
├── main.c              # Server main entry point and message handling
├── device.c            # Core device management implementation
├── bind.c              # Device binding and registration logic
├── buf.c               # Buffer management utilities
├── devman.h            # Main header with core data structures
├── devinfo.h           # Device information definitions
├── proto.h             # Function prototypes
└── Makefile            # Build configuration
```

## Core Components

### Device Management (`device.c`)
Implements the core device lifecycle management:
- Device creation and destruction
- Device state management (UNBOUND, BOUND, ZOMBIE)
- Device hierarchy maintenance
- Reference counting for safe device access
- Device information serialization

### Device Binding (`bind.c`)
Handles device registration and binding:
- Driver device registration
- Device name resolution
- Owner endpoint management
- Device capability verification

### Buffer Management (`buf.c`)
Provides efficient buffer handling:
- Dynamic buffer allocation
- String manipulation utilities
- Device information formatting

## Data Structures

### Device Structure (`devman_device`)
```c
struct devman_device {
    int dev_id;                    /* Unique device identifier */
    char *name;                    /* Device name */
    int ref_count;                 /* Reference counter */
    int major;                     /* Major device number */
    int state;                     /* Device state (UNBOUND/BOUND/ZOMBIE) */
    endpoint_t owner;              /* Owning process endpoint */
    struct devman_inode inode;     /* VFS inode association */
    struct devman_device *parent;  /* Parent device */
    struct devman_device_info *info; /* Device information */
    /* ... tree structure fields ... */
};
```

### Device States
- `DEVMAN_DEVICE_UNBOUND`: Device exists but not bound to driver
- `DEVMAN_DEVICE_BOUND`: Device is active and bound to driver
- `DEVMAN_DEVICE_ZOMBIE`: Device is being removed

### VFS Integration
The device manager integrates with MINIX VFS through:
- Custom inode types for device information
- Read functions for device data access
- Event queues for device change notifications
- Hierarchical filesystem representation

## Message Interface

### Key Message Types
- `DEVMAN_DEV_ADD`: Register new device
- `DEVMAN_DEV_REMOVE`: Unregister device
- `DEVMAN_DEV_QUERY`: Query device information
- `DEVMAN_DEV_BIND`: Bind device to driver
- `DEVMAN_DEV_UNBIND`: Unbind device from driver

### Communication Pattern
- Synchronous request-response for device queries
- Asynchronous notifications for device events
- Endpoint-based security and access control

## Security and Access Control

### Permission Model
- Device access controlled by endpoint permissions
- Owner-based device modification rights
- System-wide device visibility policies
- Capability-based device operations

### Safety Features
- Reference counting prevents premature device deletion
- State validation for all device operations
- Graceful handling of driver crashes
- Automatic cleanup of orphaned devices

## RISC-V Specific Considerations

### Memory-Mapped I/O Integration
- RISC-V uses memory-mapped I/O exclusively
- Device manager coordinates with MMIO device registration
- PLIC (Platform-Level Interrupt Controller) device management
- UART device registration for console access

### Device Tree Support
- RISC-V platforms use device tree for hardware description
- Device manager parses and registers device tree devices
- Dynamic device discovery based on device tree entries
- Platform-specific device initialization

### QEMU virt Platform
- Standard device registration for QEMU virt machines
- VirtIO device management
- CLINT and PLIC device coordination
- Memory-mapped device address space management

## Integration with Other Servers

### VFS (Virtual File System)
- Provides device information through /dev interface
- Virtual filesystem for device metadata
- Device event file notifications

### PM (Process Manager)
- Process-device ownership tracking
- Device access permission validation
- Cleanup on process termination

### RS (Reincarnation Server)
- Driver restart coordination
- Device re-registration after driver crash
- Service dependency management

## Testing and Debugging

### Debug Features
- Device tree visualization
- Device state tracking
- Reference count monitoring
- Event logging for device changes

### Common Operations
- List all registered devices
- Query device information
- Monitor device events
- Debug device hierarchy

## Build Configuration

### Makefile Targets
- Standard MINIX server build process
- Integration with system.conf for service registration
- Dependency on VFS and system libraries

### System Integration
- Service definition in `/etc/system.conf`
- IPC permission configuration
- Startup sequence coordination
- Resource allocation policies

## Future Enhancements

### Planned Features
- Hot-plug device support
- Advanced power management
- Device capability framework
- Enhanced security policies
- Dynamic driver loading

### RISC-V Optimizations
- Platform-specific device optimization
- Enhanced interrupt management
- Advanced PLIC integration
- Support for future RISC-V extensions

This architecture provides a robust foundation for device management in MINIX 3, with particular attention to the needs of embedded and RISC-V platforms while maintaining the system's microkernel principles and fault tolerance.