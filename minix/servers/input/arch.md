# MINIX Input Server Architecture

This directory contains the Input Server implementation, which coordinates keyboard and mouse input devices in MINIX. The input server acts as a central hub for input event management, providing multiplexing capabilities for multiple input devices and distributing events to appropriate consumers.

## Core Input Management Files

### Server Infrastructure
- **input.c**: Main input server implementation
  - Server initialization and setup
  - Message loop and event dispatch
  - Device registration and management
  - Input event buffering and queuing
  - Device multiplexing logic
  - Event distribution to consumers
  - Synchronization and flow control
  - Error handling and recovery

- **input.h**: Input server definitions
  - Input device constants and macros
  - Device minor number mappings
  - Event buffer configuration
  - Device state structure definitions
  - Input event type definitions
  - Buffer size and queue parameters

### Input Device Management
The input server supports multiple device types with dedicated minor device numbers:

**Keyboard Devices:**
- **KBDMUX_MINOR (0)**: Keyboard multiplexer device
- **KBD0_MINOR (1)**: First physical keyboard device
- **KBD_MINORS (4)**: Maximum number of keyboard devices
- **KBDMUX_DEV**: Array index for keyboard multiplexer
- **FIRST_KBD_DEV**: Array index for first keyboard device
- **LAST_KBD_DEV**: Array index for last keyboard device

**Mouse Devices:**
- **MOUSEMUX_MINOR (64)**: Mouse multiplexer device
- **MOUSE0_MINOR (65)**: First physical mouse device
- **MOUSE_MINORS (4)**: Maximum number of mouse devices
- **MOUSEMUX_DEV**: Array index for mouse multiplexer
- **FIRST_MOUSE_DEV**: Array index for first mouse device
- **LAST_MOUSE_DEV**: Array index for last mouse device

### Input Device State Structure
- **struct input_dev**: Input device state structure
  - `minor`: Minor number of this device
  - `owner`: Owning driver endpoint, or NONE
  - `label`: Label of owning driver from DS
  - `eventbuf[EVENTBUF_SIZE]`: Circular event buffer
  - `tail`: Tail pointer into ring buffer
  - `count`: Number of elements in ring buffer
  - `opened`: Device open status flag
  - `suspended`: Read suspension status
  - `caller`: Endpoint for suspended read operation
  - `grant`: Grant ID for suspended read buffer
  - `req_id`: Request ID for suspended read
  - `selector`: Read-selecting endpoint, or NONE
  - `leds`: LED mask - preserved across driver connections

### Event Buffer Management
- **EVENTBUF_SIZE (32)**: Maximum events per device buffer
- **input_dev_buf_empty()**: Check if buffer is empty
- **input_dev_buf_full()**: Check if buffer is full
- **input_dev_active()**: Check if device is active

## Input Server Operations

### Device Open and Close
- **input_open()**: Open input device for access
  - Validate minor device number
  - Check device availability and ownership
  - Prevent multiple simultaneous opens
  - Initialize device state for new client

- **input_close()**: Close input device
  - Release device resources
  - Clear pending operations
  - Reset device state
  - Notify waiting processes

### Event Reading and Processing
- **input_read()**: Read input events from device
  - Retrieve events from circular buffer
  - Handle blocking and non-blocking reads
  - Support partial reads and buffering
  - Implement flow control mechanisms
  - Manage read suspension and resumption

- **input_cancel()**: Cancel pending read operation
  - Abort suspended read requests
  - Clean up associated resources
  - Notify waiting processes of cancellation

### Device Control and Configuration
- **input_ioctl()**: Control input device parameters
  - LED state management for keyboards
  - Device configuration and querying
  - Event filtering and processing options
  - Device capability discovery

- **input_select()**: Select/poll support for input devices
  - Report device readability status
  - Support for multiple device monitoring
  - Event-driven I/O notification

### Event Distribution
- **input_other()**: Handle miscellaneous messages
  - Process driver event notifications
  - Handle device registration messages
  - Coordinate device state changes
  - Manage event routing and distribution

## Message Passing Interface

### Input Server Messages
The input server processes various message types for device management and event distribution:

**Device Control Messages:**
- Device open/close requests from user processes
- Read requests for input events
- IOCTL commands for device configuration
- Select/poll requests for event notification

**Driver Communication Messages:**
- Event notifications from input drivers
- Device registration and deregistration
- Status updates and error reports
- Configuration change notifications

### IPC Protocols
**Client to Input Server:**
- Synchronous request-response for device operations
- Asynchronous event notifications via select/poll
- Grant-based data transfer for large event buffers

**Input Server to Drivers:**
- Device capability queries
- Configuration parameter updates
- Status and health monitoring requests

## Integration with MINIX System

### Driver Coordination
The input server works closely with hardware drivers:
- **TTY Drivers**: Coordinate with serial/keyboard drivers
- **USB Drivers**: Interface with USB HID device drivers
- **PS/2 Drivers**: Handle legacy PS/2 keyboard and mouse
- **Virtual Drivers**: Support virtual input devices

### System Service Integration
- **VFS**: Register as character device provider
- **DS**: Store and retrieve driver labels and capabilities
- **RS**: Participate in system restart and recovery
- **PM**: Coordinate with process management for device access

### Device Registration Process
1. Input drivers register with the input server
2. Server validates driver capabilities and requirements
3. Device minor numbers are assigned and mapped
4. Event buffer and state structures are initialized
5. Device becomes available for user process access

## RISC-V Specific Implementation

### Platform Considerations
The input server is designed to be platform-independent, but includes RISC-V specific optimizations:

**Memory Alignment:**
- Event structures are properly aligned for RISC-V memory requirements
- Buffer management follows RISC-V atomic operation constraints
- Grant-based data transfer optimized for RISC-V virtual memory

**Interrupt Coordination:**
- Works with RISC-V PLIC for device interrupt handling
- Coordinates with SBI for timer-based event processing
- Supports RISC-V specific I/O memory mapping

**Performance Optimizations:**
- Efficient circular buffer implementation for RISC-V cache hierarchy
- Optimized event copying for RISC-V memory bandwidth characteristics
- Minimal overhead in critical event processing paths

## Configuration and Build

### Build Configuration
- **Makefile**: Server build configuration
  - Links with libchardriver for character device support
  - Links with libsys for system services
  - Uses standard MINIX service build rules

### System Configuration
The input server is configured in `/etc/system.conf` with:
- IPC permissions for communication with drivers and clients
- System call privileges for device management
- Memory access rights for event buffers
- I/O port ranges for hardware device access

## Development Guidelines

### Adding New Input Device Types
1. Define new minor device numbers in input.h
2. Update device mapping functions in input.c
3. Implement device-specific event handling
4. Add IOCTL commands for device configuration
5. Update system configuration files

### Event Processing Optimization
- Minimize event processing latency in critical paths
- Use efficient circular buffer algorithms
- Implement proper flow control mechanisms
- Handle edge cases in buffer management

### Error Handling
- Validate all device operations and parameters
- Handle driver disconnection gracefully
- Implement proper resource cleanup
- Provide meaningful error codes to clients

### Testing and Debugging
- Test with multiple simultaneous input devices
- Verify event ordering and timing
- Test buffer overflow and underflow conditions
- Validate device hot-plug and removal scenarios

## Fault Tolerance

### Crash Recovery
The input server supports MINIX's fault tolerance model:
- Server restart preserves device configuration
- Event buffers are reinitialized on recovery
- Driver reconnection is handled automatically
- Client processes are notified of server restart

### Resource Management
- Proper cleanup of device resources on errors
- Prevention of resource leaks in error conditions
- Graceful degradation when resources are limited
- Memory allocation failure handling

This architecture enables reliable input device management in MINIX's microkernel environment, providing a robust foundation for user interaction while maintaining system stability and fault tolerance.