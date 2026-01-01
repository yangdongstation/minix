# MINIX RISC-V Board Support Package

This directory contains the Board Support Package (BSP) for RISC-V platforms, providing platform-specific initialization and hardware abstraction for different RISC-V development boards and virtual platforms.

## Platform Support Structure

### QEMU virt Platform (virt/)
The primary RISC-V platform supported by MINIX:

#### Platform Initialization
- **bsp_init.c**: Board initialization code
  - Platform detection and validation
  - Memory map configuration
  - Device tree parsing
  - Platform capability detection
  - Early hardware initialization
  - Platform-specific setup

#### Interrupt Management
- **bsp_intr.c**: Board-specific interrupt handling
  - Platform interrupt routing
  - IRQ vector configuration
  - Interrupt priority setup
  - Platform-specific interrupt handling
  - Interrupt controller initialization
  - Cross-platform interrupt abstraction

#### Timer Support
- **bsp_timer.c**: Platform timer implementation
  - Hardware timer detection
  - Timer frequency calculation
  - Clock source initialization
  - Timer interrupt configuration
  - Platform-specific timing services
  - Timer calibration

#### Serial Console
- **bsp_serial.c**: Serial port initialization
  - UART device detection
  - Serial console setup
  - Baud rate configuration
  - Early debug output
  - Platform-specific console support
  - Multi-UART management

#### System Control
- **bsp_reset.c**: System reset handling
  - Platform reset sequences
  - Clean shutdown procedures
  - Hardware reset coordination
  - Platform-specific reset handling
  - Emergency restart support

#### Build Configuration
- **Makefile.inc**: Platform build settings
  - Compilation flags for virt platform
  - Platform-specific optimizations
  - Debug configuration
  - Platform detection macros

## Platform Abstraction Interface

The BSP provides a clean abstraction layer between the generic RISC-V architecture code and platform-specific implementations:

### Initialization Sequence
1. **Early Platform Detection**: Identify platform type and capabilities
2. **Memory Map Setup**: Configure platform-specific memory layout
3. **Device Initialization**: Initialize platform devices (UART, timers, etc.)
4. **Interrupt Configuration**: Set up platform interrupt routing
5. **Console Setup**: Initialize early console for debug output
6. **Timer Calibration**: Set up platform timing services

### Hardware Detection
- Automatic platform detection
- Device tree parsing for configuration
- Capability enumeration
- Resource allocation
- Conflict resolution

### Platform-Specific Features
- **QEMU virt Platform**:
  - VirtIO device support
  - PCI Express enumeration
  - Multiple serial ports
  - Network interface detection
  - Block device initialization
  - Graphics support

## Integration with MINIX

### Message-Based Interface
The BSP integrates with MINIX's message-passing architecture:
- Structured initialization messages
- Asynchronous hardware event notification
- Platform capability reporting
- Error handling through IPC

### Security Considerations
- Platform resource validation
- Hardware access permission checking
- Secure boot verification
- Platform integrity monitoring

### Performance Optimization
- Platform-specific optimizations
- Hardware feature utilization
- Memory layout optimization
- Interrupt handling efficiency

## Development and Extension

### Adding New Platforms
To add support for a new RISC-V platform:
1. Create new platform subdirectory
2. Implement platform-specific initialization
3. Define memory map and device configuration
4. Set up interrupt routing
5. Configure timer and console support
6. Add build configuration

### Platform Detection
The BSP supports multiple RISC-V platforms through:
- Runtime platform detection
- Compile-time platform selection
- Modular platform support
- Dynamic platform capabilities

## Testing and Validation

### Platform Testing
- Hardware compatibility verification
- Memory map validation
- Interrupt functionality testing
- Timer accuracy verification
- Console output validation

### Cross-Platform Compatibility
- Consistent interface across platforms
- Platform-agnostic kernel code
- Hardware abstraction verification
- Portability testing

This BSP architecture enables MINIX to support multiple RISC-V platforms while maintaining a clean separation between generic RISC-V architecture code and platform-specific implementations, ensuring portability and maintainability across different hardware implementations.