# MINIX System Commands Architecture

This directory contains MINIX-specific system commands and utilities that are essential for system administration, configuration, and maintenance. These commands are unique to MINIX and complement the standard NetBSD utilities found in the root-level directories.

## System Management Commands

### Service Management
- **minix-service/**: MINIX service management tool
  - **minix-service.c**: Main service control program
  - **parse.c/parse.h**: Command-line parsing
  - **print.c**: Output formatting
  - **util.c**: Utility functions
  - **minix-service.8**: Manual page
  - **system.conf.5**: Configuration file documentation
  - Controls system servers and drivers
  - Start, stop, restart, and query services
  - Service dependency management

### System Control
- **svrctl/**: Server control utility
  - **svrctl.c**: Direct server control interface
  - Send control messages to system servers
  - Query server status and configuration
  - Emergency server management

### System Updates
- **update/**: System update command
  - **update.c**: Simple update utility
  - Apply system updates and patches
  - Coordinate with update servers

- **update_asr/**: Update ASR (Automatic System Recovery)
- **updateboot/**: Boot loader update
- **update_bootcfg/**: Boot configuration update

### System Information
- **sysenv/**: System environment display
- **readclock/**: Hardware clock reading
- **version/**: System version information
- **printroot/**: Root device information

## Storage and File System Management

### Disk Management
- **fdisk/**: Fixed disk partition editor
- **format/**: Disk formatting utility
- **devsize/**: Device size reporting
- **partition/**: Partition management
- **repartition/**: Advanced partitioning
- **autopart/**: Automatic partitioning

### File System Operations
- **fsck.mfs/**: MINIX file system check
- **mount/**: File system mounting
- **umount/**: File system unmounting
- **truncate/**: File truncation

### Backup and Recovery
- **backup/**: System backup utility
- **remsync/**: Remote synchronization

## Device Management

### Device Configuration
- **MAKEDEV/**: Device node creation
- **lspci/**: PCI device listing
- **cdprobe/**: CD-ROM probing
- **eject/**: Media ejection

### Hardware Control
- **intr/**: Interrupt management
- **ramdisk/**: RAM disk operations
- **loadramdisk/**: RAM disk loading

## Network Configuration

### Network Setup
- **netconf/**: Network configuration
- **slip/**: SLIP protocol setup
- **swifi/**: Wireless network management
  - Comprehensive wireless configuration
  - WPA/WPA2 support
  - Access point scanning

## Performance and Monitoring

### Performance Analysis
- **profile/**: System profiling
- **sprofalyze/**: Profile analysis
- **sprofdiff/**: Profile comparison
- **rawspeed/**: Raw performance testing

### Benchmarking
- **dhrystone/**: Dhrystone benchmark
- **worldstone/**: Worldstone benchmark

## Text Processing and Utilities

### Text Processing
- **cawf/**: Computer Aided Writing Facility
- **spell/**: Spell checking
- **ifdef/**: Conditional compilation processing
- **look/**: Word lookup
- **term/**: Terminal information
- **termcap/**: Terminal capability database

### Compression
- **compress/**: File compression
- **crc/**: CRC calculation

## Audio Support

### Audio Utilities
- **playwave/**: Wave file playback
- **recwave/**: Wave file recording

## System Maintenance

### System Cleanup
- **cleantmp/**: Temporary file cleanup
- **rotate/**: Log file rotation

### System Preparation
- **prep/**: System preparation
- **setup/**: System setup utility

## File Operations

### Special File Systems
- **isoread/**: ISO 9660 file system reading
- **dosread/**: DOS file system reading
- **writeisofs/**: ISO file system creation

### File Transfer
- **zmodem/**: ZMODEM file transfer protocol

## Display and Fonts

### Display Utilities
- **screendump/**: Screen capture
- **loadfont/**: Font loading
- **loadkeys/**: Keyboard layout loading

### Progress Display
- **progressbar/**: Progress indication

## Development Tools

### Code Analysis
- **srccrc/**: Source code CRC checking

### System Building
- **fetch/**: File fetching utility
- **fix/**: File repair utility

## Communication

### Messaging
- **mail/**: Mail handling
- **cron/**: Cron daemon
- **crontab/**: Cron table editor
- **at/**: At job scheduler
- **atnormalize/**: At job normalization

## Mathematical and Scientific

### Volume Calculation
- **vol/**: Volume calculation

## Service Management

### Device Management Daemon
- **devmand/**: Device manager daemon
  - Device enumeration and management
  - Driver coordination
  - Hardware monitoring

## Build Configuration

### Makefiles
- **Makefile**: Top-level build configuration
- **Makefile.inc**: Common build definitions
- **DESCRIBE**: Package description file

## Command Categories

### System Administration
Commands for system maintenance and configuration:
- Service management (minix-service, svrctl)
- System updates (update, update_*)
- Storage management (fdisk, format, mount)
- Device management (MAKEDEV, lspci)

### Network Management
Network configuration and monitoring:
- Network setup (netconf)
- Wireless management (swifi)
- Protocol support (slip)

### Performance Monitoring
System performance and analysis:
- Profiling tools (profile, sprofalyze)
- Benchmarking (dhrystone, worldstone)
- Performance testing (rawspeed)

### File System Tools
File system and storage utilities:
- File system checking (fsck.mfs)
- Partition management (fdisk, partition)
- Backup and sync (backup, remsync)

### Hardware Support
Hardware-specific utilities:
- Device probing (cdprobe, intr)
- Hardware control (loadramdisk, readclock)
- Media management (eject)

## Integration with System Services

### Service Communication
Many commands communicate with system servers:
- **minix-service**: Primary interface to service management
- **svrctl**: Direct server control
- Service-specific commands for configuration

### Configuration Files
Commands work with system configuration:
- **/etc/system.conf**: Service configuration
- **/etc/fstab**: File system table
- **/etc/network.conf**: Network configuration

## Development Guidelines

### Command Design
- Follow MINIX message-passing paradigm
- Integrate with system servers appropriately
- Provide meaningful error messages
- Support standard command-line conventions

### Error Handling
- Proper error reporting
- Exit codes following conventions
- Graceful failure handling
- User-friendly error messages

### Manual Pages
- Comprehensive documentation
- Usage examples
- Configuration details
- Integration notes

These MINIX-specific commands provide essential system management capabilities while maintaining consistency with the microkernel architecture and message-passing design philosophy.