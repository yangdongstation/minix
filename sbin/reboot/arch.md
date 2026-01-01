# MINIX System Reboot Utility (reboot)

## Overview

The `reboot` utility is a critical system administration tool for controlling system shutdown and restart operations in MINIX systems. It provides comprehensive system control capabilities, including graceful shutdown sequences, emergency reboot functions, and coordination with system services. This utility is essential for system maintenance, configuration changes, and recovery operations, serving as the primary interface for system state transitions.

## Architecture

### Core Functionality
- **System Reboot**: Controlled system restart operations
- **Shutdown Coordination**: Graceful service termination
- **Emergency Operations**: Force reboot capabilities
- **Runlevel Management**: System state transitions
- **Signal Broadcasting**: System-wide shutdown notifications
- **Process Management**: Application termination coordination
- **Hardware Control**: Low-level system reset

### MINIX Integration
- Communicates with PM server for process management
- Uses MINIX message-passing for system calls
- Integrates with system initialization (init)
- Coordinates with reincarnation server (RS)
- Supports MINIX service-oriented shutdown

## Source Files

### Main Implementation
- **`reboot.c`** (6,568 lines)
  - Main program entry point and command dispatcher
  - Command-line argument parsing and validation
  - System call interface for reboot operations
  - Shutdown sequence coordination
  - Signal handling and process termination
  - Hardware reset interface
  - Error handling and diagnostics
  - Integration with MINIX system services

### Manual and Documentation
- **`reboot.8`** (4,875 lines)
  - Comprehensive manual page
  - Usage examples and options
  - Shutdown procedures
  - System administration guidelines
  - Emergency procedures
  - Security considerations

### Build Configuration
- **`Makefile`** (345 lines)
  - Standard MINIX build configuration
  - Source file compilation rules
  - Library dependencies and linking
  - Installation targets
  - Cross-compilation support

## Build Configuration

### Dependencies
- **System Libraries**: libc, libutil, libkvm
- **MINIX Libraries**: libminix, libsys, libtimers
- **Header Files**: sys/reboot.h, signal.h, unistd.h
- **Process Management**: PM server interface

### Build Requirements
- C compiler with C99 support
- MINIX cross-compilation toolchain
- System call interface definitions
- Hardware control headers

## MINIX System Integration

### Server Communication
```c
// Reboot system call interface
int reboot(int howto, char *bootstr);

// Message-based communication with PM
message m;
m.m_type = PM_REBOOT;
m.PM_REBOOT_HOW = howto;
m.PM_REBOOT_STRING = bootstr;
_sendrec(PM_PROC_NR, &m);
```

### Service Dependencies
- **PM Server**: Primary process management interface
- **RS Server**: Reincarnation server coordination
- **INIT**: System initialization process (PID 1)
- **DS Server**: Service discovery and shutdown
- **TTY Server**: Console notification

### System Shutdown Sequence
```c
// Shutdown coordination
void shutdown_sequence(int howto) {
    // 1. Notify system services
    broadcast_shutdown_signal();
    
    // 2. Terminate user processes
    terminate_processes();
    
    // 3. Sync filesystems
    sync_filesystems();
    
    // 4. Perform reboot
    perform_reboot(howto);
}
```

## Reboot Operation Process

### Graceful Reboot Sequence
1. **Signal Broadcasting**: Send SIGTERM to processes
2. **Process Termination**: Wait for process cleanup
3. **Service Shutdown**: Coordinate with system services
4. **Filesystem Sync**: Flush all pending I/O
5. **System Preparation**: Prepare for reboot
6. **Hardware Reset**: Perform system reset

### Emergency Reboot Sequence
1. **Immediate Signal**: Send SIGKILL to processes
2. **Forced Sync**: Emergency filesystem sync
3. **Hardware Reset**: Immediate system reset

### Reboot Options and Flags
- **RB_AUTOBOOT**: Automatic reboot (default)
- **RB_HALT**: Halt system without reboot
- **RB_POWEROFF**: Power off system
- **RB_NOSYNC**: Don't sync filesystems
- **RB_RADIOSILENT**: Silent reboot
- **RB_DUMP**: Generate system dump

## System State Management

### Runlevel Transitions
```c
// System runlevel management
int set_runlevel(int newlevel);
int get_current_runlevel(void);

// Runlevel definitions
#define RUNLEVEL_SINGLE     1
#define RUNLEVEL_MULTIUSER  2
#define RUNLEVEL_REBOOT     6
#define RUNLEVEL_HALT       0
```

### Process Termination
- **SIGTERM**: Graceful termination request
- **SIGKILL**: Forceful termination
- **Process Groups**: Handle process groups
- **Session Leaders**: Terminal session management
- **Zombie Processes**: Clean up terminated processes

### Filesystem Coordination
```c
// Filesystem sync operations
void sync_filesystems(void);
void emergency_sync(void);
int sync_and_umount_all(void);
```

## Shutdown Coordination

### Service Notification
```c
// Broadcast shutdown to services
void broadcast_shutdown_signal(void) {
    // Notify system servers
    notify_server(RS_PROC_NR, SIG_SHUTDOWN);
    notify_server(VFS_PROC_NR, SIG_SHUTDOWN);
    notify_server(PM_PROC_NR, SIG_SHUTDOWN);
    notify_server(INET_PROC_NR, SIG_SHUTDOWN);
    // ... other services
}
```

### Process Hierarchy Management
- **Init Process**: PID 1 coordination
- **Session Leaders**: Terminal session cleanup
- **Process Groups**: Group signal handling
- **Orphan Processes**: Proper cleanup
- **Daemon Processes**: Service shutdown

### Hardware Interface
```c
// Hardware control interface
int hardware_reset(void);
int power_off_system(void);
int halt_processor(void);
```

## RISC-V 64-bit Platform Considerations

### Architecture Independence
- Portable C implementation
- No assembly code dependencies
- Platform-agnostic algorithms
- Cross-architecture compatibility

### RISC-V Specific Features
- Memory-aligned data structures
- Efficient system call interface
- RISC-V hardware reset sequences
- Platform-specific power management

### Hardware Reset Implementation
- **SBI Interface**: Supervisor Binary Interface
- **Reset Commands**: Platform-specific reset
- **Power Management**: RISC-V power states
- **Memory Barriers**: Architecture requirements

### Platform Integration
- QEMU virt platform support
- Hardware abstraction layer
- Platform-specific optimizations
- Cross-platform compatibility

## Error Handling and Diagnostics

### Error Categories
- **Permission Errors**: Insufficient privileges
- **System Errors**: Kernel-level failures
- **Service Errors**: Server communication problems
- **Hardware Errors**: Reset mechanism failures
- **Process Errors**: Termination problems

### Error Recovery
```c
// Error handling strategy
int handle_reboot_error(int error) {
    switch (error) {
        case EPERM:
            fprintf(stderr, "reboot: permission denied\n");
            return EXIT_FAILURE;
        case EBUSY:
            fprintf(stderr, "reboot: system busy\n");
            return emergency_reboot();
        case EIO:
            fprintf(stderr, "reboot: I/O error\n");
            return hardware_reset();
        default:
            perror("reboot");
            return EXIT_FAILURE;
    }
}
```

### Diagnostic Features
- **Verbose Mode**: Detailed operation reporting
- **Debug Output**: Internal operation tracing
- **System State**: Current system information
- **Error Logging**: Persistent error records

## Security Considerations

### Access Control
- Root privilege requirement for reboot operations
- Capability-based access control
- Audit logging for system changes
- Secure system state transitions

### System Protection
- **Privilege Verification**: Confirm appropriate privileges
- **Authentication**: User identity validation
- **Authorization**: Operation permission checking
- **Audit Trail**: Operation logging

### Secure Shutdown
- **Process Validation**: Verify legitimate shutdown
- **Service Coordination**: Secure service termination
- **Data Protection**: Ensure data integrity
- **Hardware Safety**: Safe hardware operations

## Usage Examples

### Basic Reboot Operations
```bash
# Graceful system reboot
reboot

# Halt system (no reboot)
reboot -h

# Power off system
reboot -p

# Force immediate reboot
reboot -f
```

### Advanced Usage
```bash
# Reboot with diagnostic
reboot -d

# Reboot without syncing
reboot -n

# Reboot with custom boot string
reboot -k "custom boot"

# Emergency reboot
reboot -q
```

### System Administration
```bash
# Scheduled reboot
shutdown -r +5 "System reboot in 5 minutes"

# Immediate shutdown
halt -p

# Single-user mode
init 1
```

## Testing and Validation

### Functional Testing
- Graceful reboot operations
- Emergency reboot functionality
- Error condition handling
- System state verification

### Stress Testing
- Concurrent shutdown requests
- Process termination stress
- Filesystem sync testing
- Hardware reset validation

### Platform Testing
- RISC-V 64-bit validation
- QEMU platform testing
- Hardware reset verification
- Cross-platform compatibility

## Performance Optimization

### Shutdown Speed
- Fast process termination
- Efficient filesystem sync
- Optimized service shutdown
- Minimal overhead operations

### Resource Management
- Memory cleanup during shutdown
- CPU usage optimization
- I/O bandwidth management
- System responsiveness

### Reliability Features
- **Graceful Degradation**: Handle partial failures
- **Recovery Mechanisms**: Fallback procedures
- **Error Resilience**: Continue despite errors
- **State Consistency**: Maintain system integrity

## Maintenance and Updates

### NetBSD Synchronization
- Regular updates from NetBSD
- Security patches and fixes
- Hardware support updates
- Performance improvements

### MINIX-Specific Enhancements
- Service-oriented shutdown
- Message-based coordination
- Enhanced reliability features
- Microkernel optimization

### Future Development
- Advanced power management
- Enhanced diagnostics
- Better error recovery
- Platform-specific optimizations

The `reboot` utility provides comprehensive system control capabilities essential for MINIX system administration. Its robust shutdown coordination, multiple reboot options, and tight integration with MINIX system services make it a critical component for system management and maintenance in the microkernel environment.