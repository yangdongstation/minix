# MINIX System Shutdown Utility (shutdown)

## Overview

The `shutdown` utility is a sophisticated system administration tool for managing graceful system shutdown and restart operations in MINIX systems. It provides comprehensive system state management capabilities, including scheduled shutdowns, user notifications, service coordination, and emergency procedures. This utility is essential for system maintenance, planned outages, and coordinated system transitions, serving as the primary interface for controlled system shutdown sequences.

## Architecture

### Core Functionality
- **Scheduled Shutdown**: Time-based shutdown scheduling
- **User Notification**: System-wide user warnings
- **Service Coordination**: Graceful service termination
- **Runlevel Management**: System state transitions
- **Emergency Procedures**: Quick shutdown capabilities
- **Process Management**: Application termination coordination
- **Logging and Auditing**: Comprehensive operation logging

### MINIX Integration
- Communicates with init process (PID 1) for system control
- Uses MINIX message-passing for system calls
- Integrates with system services for coordinated shutdown
- Supports MINIX runlevel management
- Coordinates with PM server for process termination

## Source Files

### Main Implementation
- **`shutdown.c`** (12,668 lines)
  - Main program entry point and command dispatcher
  - Command-line argument parsing and validation
  - Shutdown scheduling and timing logic
  - User notification and warning systems
  - Service coordination and termination
  - System call interface for shutdown operations
  - Signal handling and process management
  - Logging and audit functionality
  - Integration with MINIX system services

### Configuration and Paths
- **`pathnames.h`** (2,018 lines)
  - System path definitions
  - Configuration file locations
  - Default file paths
  - System directory specifications
  - Runtime file locations

### Build Configuration
- **`Makefile`** (218 lines)
  - Standard MINIX build configuration
  - Source file compilation rules
  - Library dependencies and linking
  - Installation targets
  - Cross-compilation support

### Manual and Documentation
- **`shutdown.8`** (6,427 lines)
  - Comprehensive manual page
  - Usage examples and options
  - Shutdown procedures
  - System administration guidelines
  - Scheduling and timing options
  - Emergency procedures
  - Security considerations

## Build Configuration

### Dependencies
- **System Libraries**: libc, libutil, libkvm
- **MINIX Libraries**: libminix, libsys, libtimers
- **Header Files**: sys/time.h, signal.h, unistd.h, syslog.h
- **Process Management**: PM and INIT server interfaces

### Build Requirements
- C compiler with C99 support
- MINIX cross-compilation toolchain
- System call interface definitions
- Time and scheduling headers

## MINIX System Integration

### Server Communication
```c
// Init process communication
message m;
m.m_type = INIT_SHUTDOWN;
m.INIT_SHUTDOWN_TIME = shutdown_time;
m.INIT_SHUTDOWN_FLAGS = shutdown_flags;
_sendrec(INIT_PROC_NR, &m);
```

### Service Dependencies
- **INIT Process**: Primary system control interface (PID 1)
- **PM Server**: Process management and termination
- **RS Server**: Reincarnation server coordination
- **TTY Server**: Console and terminal notification
- **VFS Server**: Filesystem sync and unmount
- **DS Server**: Service discovery and coordination

### System Shutdown Sequence
```c
// Coordinated shutdown sequence
void perform_shutdown(int howto, time_t when) {
    // 1. Schedule shutdown
    schedule_shutdown(when);
    
    // 2. Notify users
    broadcast_shutdown_warning(when);
    
    // 3. Wait for grace period
    wait_for_grace_period();
    
    // 4. Terminate services
    shutdown_system_services();
    
    // 5. Sync and unmount
    sync_filesystems_and_unmount();
    
    // 6. Final shutdown
    execute_final_shutdown(howto);
}
```

## Shutdown Operation Process

### Scheduled Shutdown Sequence
1. **Time Parsing**: Parse shutdown time specification
2. **Privilege Checking**: Verify root privileges
3. **Warning Broadcast**: Notify logged-in users
4. **Service Notification**: Inform system services
5. **Grace Period**: Wait for specified time
6. **Process Termination**: Terminate user processes
7. **System Sync**: Flush filesystem buffers
8. **Final Shutdown**: Execute shutdown or reboot

### Immediate Shutdown Sequence
1. **Emergency Notification**: Quick user warning
2. **Service Termination**: Immediate service shutdown
3. **Process Kill**: Force process termination
4. **Filesystem Sync**: Emergency sync
5. **System Shutdown**: Execute final action

### Shutdown Options and Modes
- **Halt (-h)**: Stop system without power off
- **Power Off (-p)**: Power down system
- **Reboot (-r)**: Restart system
- **Single User (-s)**: Go to single-user mode

## Time Management and Scheduling

### Time Specification
```c
// Time parsing and validation
int parse_shutdown_time(const char *timestr, time_t *when);
int parse_relative_time(const char *timestr, time_t *when);

// Time formats supported
// +minutes - Relative minutes from now
// HH:MM - Absolute time (24-hour format)
// now - Immediate shutdown
// +1:30 - Hours and minutes
```

### Warning Intervals
- **Initial Warning**: Immediate notification
- **Progressive Warnings**: Countdown updates
- **Final Warning**: Last chance notification
- **Wall Messages**: System-wide broadcasts

### Scheduling Algorithm
```c
// Shutdown scheduling logic
void schedule_shutdown(time_t target_time) {
    time_t current_time = time(NULL);
    time_t delay = target_time - current_time;
    
    if (delay <= 0) {
        perform_immediate_shutdown();
    } else {
        schedule_delayed_shutdown(delay);
    }
}
```

## User Notification System

### Wall Message Broadcasting
```c
// User notification system
void broadcast_shutdown_warning(time_t when, const char *message) {
    char wall_message[1024];
    snprintf(wall_message, sizeof(wall_message),
             "System shutdown scheduled for %s\n%s",
             format_time(when), message);
    
    // Broadcast to all terminals
    wall(wall_message);
    
    // Log to system log
    syslog(LOG_WARNING, "System shutdown: %s", wall_message);
}
```

### Message Customization
- **Default Messages**: Standard warning text
- **Custom Messages**: User-specified warnings
- **Progress Updates**: Countdown messages
- **Cancellation Notices**: Shutdown cancellation

### Notification Channels
- **Wall Messages**: Terminal broadcasts
- **System Log**: Syslog entries
- **Console Output**: Direct console messages
- **Email Notifications**: Optional email alerts

## Service Coordination

### System Service Shutdown
```c
// Service shutdown coordination
void shutdown_system_services(void) {
    // Notify reincarnation server
    notify_service(RS_PROC_NR, SIG_SHUTDOWN);
    
    // Stop network services
    stop_network_services();
    
    // Sync filesystems
    sync_all_filesystems();
    
    // Terminate remaining services
    terminate_remaining_services();
}
```

### Service Shutdown Order
1. **User Services**: Application-level services
2. **Network Services**: Network-dependent services
3. **Filesystem Services**: Filesystem operations
4. **Core Services**: System-critical services
5. **Reincarnation Server**: Service management

### Process Termination Strategy
- **SIGTERM**: Graceful termination request
- **Grace Period**: Time for cleanup
- **SIGKILL**: Forceful termination
- **Process Groups**: Handle process hierarchies
- **Orphan Cleanup**: Handle orphaned processes

## RISC-V 64-bit Platform Considerations

### Architecture Independence
- Portable C implementation
- No assembly dependencies
- Platform-agnostic algorithms
- Cross-architecture compatibility

### RISC-V Specific Features
- Memory-aligned data structures
- Efficient system call interface
- RISC-V hardware control sequences
- Platform-specific timing

### Platform Integration
- QEMU virt platform support
- Hardware abstraction compliance
- Platform-specific optimizations
- Cross-platform compatibility

### Performance Considerations
- Efficient scheduling algorithms
- Fast time calculations
- Minimal system overhead
- Optimized message passing

## Error Handling and Diagnostics

### Error Categories
- **Permission Errors**: Insufficient privileges
- **Time Errors**: Invalid time specifications
- **System Errors**: Kernel-level failures
- **Service Errors**: Server communication problems
- **User Errors**: Invalid command arguments

### Error Recovery
```c
// Error handling strategy
int handle_shutdown_error(int error) {
    switch (error) {
        case EPERM:
            fprintf(stderr, "shutdown: permission denied\n");
            return EXIT_FAILURE;
        case EINVAL:
            fprintf(stderr, "shutdown: invalid time specification\n");
            return EXIT_FAILURE;
        case EBUSY:
            fprintf(stderr, "shutdown: system busy\n");
            return attempt_emergency_shutdown();
        default:
            perror("shutdown");
            return EXIT_FAILURE;
    }
}
```

### Diagnostic Features
- **Verbose Mode**: Detailed operation reporting
- **Debug Output**: Internal operation tracing
- **System State**: Current system information
- **Logging**: Comprehensive operation logging

## Security Considerations

### Access Control
- Root privilege requirement for shutdown operations
- Capability-based access control
- Audit logging for system changes
- Secure system state transitions

### System Protection
- **Privilege Verification**: Confirm appropriate privileges
- **Authentication**: User identity validation
- **Authorization**: Operation permission checking
- **Audit Trail**: Comprehensive operation logging

### Secure Shutdown Practices
- **User Notification**: Ensure proper warning
- **Service Coordination**: Secure service termination
- **Data Protection**: Ensure data integrity
- **Hardware Safety**: Safe system operations

## Usage Examples

### Basic Shutdown Operations
```bash
# Shutdown and halt in 5 minutes
shutdown -h +5 "System maintenance"

# Immediate reboot
shutdown -r now

# Shutdown at specific time
shutdown -h 23:30 "End of day shutdown"

# Power off system
shutdown -p +1 "System power off"
```

### Advanced Usage
```bash
# Cancel pending shutdown
shutdown -c

# Single-user mode
shutdown -s now

# Reboot with custom message
shutdown -r +10 "Kernel upgrade - save your work"

# Emergency shutdown
shutdown -h now
```

### System Administration
```bash
# Weekly maintenance schedule
shutdown -r +60 "Weekly maintenance reboot"

# End-of-day shutdown script
0 18 * * 1-5 shutdown -h +30 "Office closing in 30 minutes"

# Graceful server restart
shutdown -r +5 "Server restart - please log off"
```

## Testing and Validation

### Functional Testing
- Scheduled shutdown operations
- Time parsing and validation
- User notification systems
- Service coordination

### Stress Testing
- Concurrent shutdown requests
- Process termination stress
- System load handling
- Emergency procedures

### Platform Testing
- RISC-V 64-bit validation
- Time zone handling
- Cross-platform compatibility
- Hardware interface testing

## Performance Optimization

### Scheduling Efficiency
- Fast time calculations
- Efficient timer management
- Minimal system overhead
- Optimized message passing

### Resource Management
- Memory usage optimization
- CPU utilization efficiency
- I/O operation minimization
- System responsiveness

### Scalability Features
- Multiple user notification
- Large system support
- Concurrent operation handling
- Performance monitoring

## Maintenance and Updates

### NetBSD Synchronization
- Regular updates from NetBSD
- Security patches and fixes
- Scheduling improvements
- Compatibility updates

### MINIX-Specific Enhancements
- Service-oriented shutdown
- Message-based coordination
- Enhanced reliability features
- Microkernel optimization

### Future Development
- Advanced scheduling options
- Enhanced user interfaces
- Better integration features
- Improved diagnostics

The `shutdown` utility provides comprehensive system shutdown management capabilities essential for MINIX system administration. Its sophisticated scheduling system, robust service coordination, and tight integration with MINIX system services make it a critical component for controlled system management and maintenance in the microkernel environment.