# MINIX Device Drivers Architecture

This directory contains device drivers that run as userspace processes in MINIX's microkernel architecture. Unlike monolithic kernels where drivers run in kernel space, MINIX drivers are isolated processes that communicate with the kernel and other system components through message passing.

## Driver Categories

### Terminal Drivers (tty/)
Serial and terminal device drivers:
- **tty/**: Generic TTY driver framework
  - Terminal line discipline
  - Character processing
  - Terminal emulation

- **ns16550/**: NS16550 UART driver (used by RISC-V)
  - **ns16550.c**: Main UART driver implementation
  - **ns16550.h**: UART register definitions and constants
  - Supports standard UART functionality for serial console
  - Handles interrupt-driven I/O
  - Configurable baud rates and line parameters

- **pty/**: Pseudo-terminal driver
  - Provides virtual terminal support
  - Used by terminal emulators and SSH

### Storage Drivers (storage/)
Block device drivers for mass storage:
- **at_wini/**: ATA/IDE hard disk driver
- **ahci/**: AHCI SATA controller driver
- **mmc/**: MMC/SD card driver
- **floppy/**: Floppy disk driver
- **virtio_blk/**: VirtIO block device driver
- **virtio_blk_mmio/**: VirtIO block MMIO variant
- **memory/**: Memory-based block device
- **ramdisk/**: RAM disk driver
- **vnd/**: Virtual node device driver
- **fbd/**: Flash block device driver

### Network Drivers (net/)
Network interface drivers:
- **lance/**: AMD LANCE Ethernet controller
- **rtl8139/**: Realtek RTL8139 Ethernet controller
- **e1000/**: Intel PRO/1000 Ethernet controller
- **virtio_net/**: VirtIO network device driver
- **loopback/**: Network loopback device

### Audio Drivers (audio/)
Sound and audio device support:
- **ac97/**: AC'97 audio codec
- **intel_hda/**: Intel High Definition Audio
- **sb16/**: Sound Blaster 16 compatibility

### Video Drivers (video/)
Graphics and display drivers:
- **vga/**: VGA-compatible display
- **vesa/**: VESA BIOS extensions
- **fb/**: Framebuffer device

### USB Drivers (usb/)
Universal Serial Bus support:
- **uhci/**: USB 1.1 UHCI controller
- **ohci/**: USB 1.1 OHCI controller
- **ehci/**: USB 2.0 EHCI controller
- **xhci/**: USB 3.0 xHCI controller
- **usb_hub/**: USB hub driver
- **usb_storage/**: USB mass storage
- **usb_keyboard/**: USB keyboard support
- **usb_mouse/**: USB mouse support

### System Drivers (system/)
System and platform devices:
- **rtc/**: Real-time clock
- **acpi/**: Advanced Configuration and Power Interface
- **speaker/**: System speaker
- **random/**: Hardware random number generator

### Human Interface Devices (hid/)
Input device support:
- **at_kbdc/**: AT keyboard controller
- **ps2_mouse/**: PS/2 mouse
- **atkbd/**: AT keyboard

### Power Management (power/)
Power and thermal management:
- **acpi_power/**: ACPI power management
- **thermal/**: Thermal monitoring

### Sensors (sensors/)
Hardware monitoring and sensors:
- **lm75/**: Temperature sensor
- **fan/**: Fan control

### Bus Controllers (bus/)
System bus controllers:
- **pci/**: PCI bus enumeration and configuration

### IOMMU Support (iommu/)
Input/Output Memory Management Unit:
- **intel_vt_d/**: Intel VT-d IOMMU
- **amdvi/**: AMD-Vi IOMMU

### Clock Drivers (clock/)
System clock and timer devices:
- **hpet/**: High Precision Event Timer
- **acpi_pm/**: ACPI Power Management Timer

### EEPROM Drivers (eeprom/)
Non-volatile memory support:
- **eeprom/**: Generic EEPROM driver

### Printer Support (printer/)
- **lp/**: Line printer driver

### VMM Guest Support (vmm_guest/)
Virtual machine guest integration:
- **vmware/**: VMware guest tools
- **qemu/**: QEMU guest agent

## Driver Development Framework

### Example Driver (examples/hello/)
Template for new driver development:
- Demonstrates basic driver structure
- Shows message handling patterns
- Provides build configuration example

## Driver Architecture

### Message-Based Communication
Drivers communicate through message passing:
- **Request Messages**: From applications or servers to drivers
- **Response Messages**: Driver replies to requests
- **Notification Messages**: Asynchronous events and status updates

### Driver Types
1. **Character Drivers**: Handle character-by-character I/O
   - Serial ports, keyboards, mice
   - TTY devices, consoles

2. **Block Drivers**: Handle block-oriented I/O
   - Disk drives, SSDs
   - Memory cards, RAM disks

3. **Network Drivers**: Handle network packets
   - Ethernet controllers
   - Wireless interfaces

### Driver Registration
Drivers register with the Device Manager:
1. Announce device capabilities
2. Request I/O resources (ports, memory, IRQs)
3. Specify supported device types
4. Define message handling interface

### Resource Management
Drivers request resources through the kernel:
- **I/O Ports**: Hardware register access
- **IRQ Lines**: Interrupt request channels
- **DMA Channels**: Direct memory access
- **Memory Regions**: Physical memory mapping

## RISC-V Specific Considerations

### NS16550 UART Driver
The RISC-V virt platform uses NS16550 UART for serial console:
- Base address: 0x10000000
- IRQ: Typically UART interrupt
- Supports standard baud rates
- Provides console output for boot messages

### VirtIO Devices
RISC-V virt platform supports VirtIO devices:
- **virtio_blk**: Block storage devices
- **virtio_net**: Network interfaces
- **virtio_rng**: Random number generator
- **virtio_console**: Console devices

### Platform Devices
RISC-V specific device support:
- **PLIC**: Platform-Level Interrupt Controller
- **CLINT**: Core Local Interruptor
- **SBI**: Supervisor Binary Interface

## Driver Development Guidelines

### Security Considerations
- Drivers run in userspace with limited privileges
- Must request explicit permissions for resources
- Cannot directly access kernel memory
- Subject to IPC filtering and monitoring

### Error Handling
- Handle hardware errors gracefully
- Provide meaningful error messages
- Support device recovery and reset
- Log important events and errors

### Performance
- Minimize interrupt latency
- Use efficient buffering strategies
- Support DMA when available
- Optimize for common usage patterns

### Portability
- Abstract hardware-specific details
- Use standard MINIX driver interfaces
- Support multiple device variants
- Provide configuration options

This architecture provides robust device support while maintaining system stability through isolation and fault containment. Drivers can crash without affecting the kernel or other system components, and can be restarted automatically by the Reincarnation Server.