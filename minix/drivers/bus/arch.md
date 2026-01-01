# MINIX Bus Controller Drivers Architecture

## Overview

Bus controller drivers in MINIX provide the foundation for device discovery and communication across different hardware buses. These drivers implement the low-level protocols required to enumerate devices, configure hardware resources, and provide access mechanisms for higher-level device drivers.

## Architecture Philosophy

### Microkernel Design Principles
Bus drivers follow MINIX's microkernel philosophy:
- **Userspace implementation**: All bus drivers run in userspace
- **Message-based communication**: Inter-process communication replaces direct hardware access
- **Resource management**: Centralized allocation of IRQ, I/O ports, and memory
- **Fault isolation**: Bus driver crashes don't affect system stability

### Bus Hierarchy
MINIX supports multiple bus architectures:
```
System Bus
├── PCI Bus (Primary system bus)
│   ├── PCI devices (NICs, storage, audio)
│   ├── PCI-to-PCI bridges
│   └── PCI-to-ISA bridges
├── I2C Bus (Low-speed serial bus)
│   ├── Sensors (temperature, pressure)
│   ├── EEPROMs
│   └── Real-time clocks
├── USB Bus (Universal Serial Bus)
│   ├── HID devices (keyboard, mouse)
│   ├── Mass storage
│   └── Audio devices
└── Platform Bus (SoC-specific)
    ├── GPIO controllers
    ├── Timer devices
    └── Interrupt controllers
```

## Directory Structure

```
minix/drivers/bus/
├── Makefile           # Build configuration for bus drivers
├── Makefile.inc       # Common make rules
├── i2c/              # I2C bus driver framework
├── pci/              # PCI bus enumeration driver
└── ti1225/           # TI PCI1225 CardBus controller
```

## PCI Bus Driver (pci)

### Purpose and Functionality
The PCI driver provides device enumeration, resource allocation, and access control for PCI devices:
- **Device discovery**: Scan PCI configuration space
- **Resource allocation**: Assign IRQ, I/O ports, and memory regions
- **Access control**: Manage device access permissions
- **Hot-plug support**: Handle device insertion/removal
- **Power management**: PCI power states and resume

### Key Source Files

#### `main.c` - PCI Driver Main Entry Point
```c
Key functions:
- main()                   // Driver initialization and message loop
- sef_cb_init()           // SEF initialization callback
- sef_cb_lu_prepare()     // Live update preparation
- sef_cb_lu_state_save()  // State saving for live update

Message handlers:
- do_init()               // Initialize PCI subsystem
- do_first_dev()          // Get first matching device
- do_next_dev()           // Get next matching device
- do_find_dev()           // Find specific device
- do_ids()                // Get device vendor/device IDs
- do_get_bar()            // Get Base Address Register
- do_get_irq()            // Get device IRQ assignment
- do_reserve()            // Reserve device for driver
- do_release()            // Release device reservation
- do_get_acl()            // Get access control list
```

#### `pci.c` - PCI Bus Implementation
```c
Core PCI functions:
- pci_init()              // Initialize PCI subsystem
- pci_scan_buses()        // Scan all PCI buses
- pci_scan_bus()          // Scan specific bus
- pci_scan_function()     // Scan device function
- pci_read_conf()         // Read configuration space
- pci_write_conf()        // Write configuration space

Device management:
- pci_first_dev()         // Find first matching device
- pci_next_dev()          // Find next matching device
- pci_find_dev()          // Find device by location
- pci_reserve_dev()       // Reserve device
- pci_release_dev()       // Release device
- pci_get_bar()           // Get Base Address Register
- pci_get_irq()           // Get IRQ assignment

Bridge handling:
- pci_handle_bridge()     // Process PCI-to-PCI bridge
- pci_handle_cardbus()    // Process CardBus bridge
- pci_handle_isa_bridge() // Process ISA bridge
```

#### `pci.h` - PCI Driver Header
Contains PCI configuration space definitions, device structures, and function prototypes.

### PCI Configuration Space
The driver manages standard PCI configuration space:
```c
struct pci_conf {
    u16_t vendor_id;      // Vendor identification
    u16_t device_id;      // Device identification
    u16_t command;        // Command register
    u16_t status;         // Status register
    u8_t  revision;       // Revision ID
    u8_t  prog_if;        // Programming interface
    u8_t  subclass;       // Device subclass
    u8_t  class;          // Device class
    u8_t  cache_line;     // Cache line size
    u8_t  latency;        // Latency timer
    u8_t  header_type;    // Header type
    u8_t  bist;           // Built-in self-test
    u32_t bar[6];         // Base Address Registers
    u32_t cardbus_cis;    // CardBus CIS pointer
    u16_t subvendor;      // Subsystem vendor ID
    u16_t subdevice;      // Subsystem device ID
    u32_t rom_addr;       // Expansion ROM address
    u8_t  int_line;       // Interrupt line
    u8_t  int_pin;        // Interrupt pin
    u8_t  min_grant;      // Minimum grant
    u8_t  max_latency;    // Maximum latency
};
```

### Device Enumeration Process
1. **Bus scanning**: Start from bus 0, scan all devices
2. **Function enumeration**: Check all 8 functions per device
3. **Bridge detection**: Handle PCI-to-PCI bridges
4. **Resource allocation**: Assign IRQ, I/O, and memory
5. **Driver matching**: Match devices to drivers
6. **Access control**: Setup device access permissions

### Resource Management
```c
/* PCI resource allocation */
struct pci_resource {
    u32_t base;           // Resource base address
    u32_t size;           // Resource size
    u32_t type;           // Resource type (I/O, memory, IRQ)
    int allocated;        // Allocation status
};

/* Resource allocation functions */
int pci_alloc_irq(int bus, int dev, int func);
int pci_alloc_io_range(u32_t size, u32_t align);
int pci_alloc_mem_range(u32_t size, u32_t align);
void pci_free_resource(struct pci_resource *res);
```

## I2C Bus Driver (i2c)

### Purpose and Functionality
The I2C driver provides serial bus communication for low-speed peripherals:
- **Device discovery**: I2C device enumeration
- **Bus arbitration**: Multi-master bus management
- **Device reservation**: Exclusive device access
- **Standard protocols**: SMBus and I2C protocol support
- **Character device interface**: Unix-style device access

### Architecture Overview
```
I2C Framework
├── Generic I2C driver (i2c.c)
├── Hardware abstraction layer
│   ├── OMAP I2C controller
│   ├── DesignWare I2C
│   └── Platform-specific drivers
├── Device reservation system
└── Character device interface
```

### Key Source Files

#### `i2c.c` - Generic I2C Driver
```c
Main functions:
- i2c_init()              // Initialize I2C subsystem
- i2c_ioctl()             // Handle I2C IOCTL requests
- i2c_other()             // Handle I2C messages

Device reservation:
- do_reserve()            // Reserve I2C device
- do_release()            // Release I2C device
- check_reservation()     // Verify device access

Character device interface:
- i2c_open()              // Open I2C device
- i2c_close()             // Close I2C device
- i2c_read()              // Read from I2C device
- i2c_write()             // Write to I2C device
```

### I2C Device Interface
```c
/* I2C IOCTL commands */
#define I2C_IOCTL_EXEC      _IOW('I', 1, struct minix_i2c_ioctl_exec)
#define I2C_IOCTL_RESERVE   _IOW('I', 2, int)
#define I2C_IOCTL_RELEASE   _IO('I', 3)

/* I2C transfer structure */
struct minix_i2c_ioctl_exec {
    i2c_addr_t slave_addr;    // Slave device address
    u16_t read_n;             // Number of bytes to read
    u16_t write_n;            // Number of bytes to write
    cp_grant_id_t read_buf;   // Grant for read buffer
    cp_grant_id_t write_buf;  // Grant for write buffer
    int flags;                // Transfer flags
};
```

### Hardware Abstraction
Platform-specific I2C controllers implement the hardware interface:
```c
/* Hardware abstraction interface */
struct i2c_hw_interface {
    int (*init)(int instance);              // Initialize hardware
    int (*process)(struct i2c_transfer *);  // Process transfer
    int (*set_speed)(u32_t speed);          // Set bus speed
    int (*reset)(void);                     // Reset controller
};
```

### Platform Support
- **OMAP I2C**: AM335x/DM37xx SoC I2C controllers
- **DesignWare**: Synopsys DesignWare I2C core
- **Platform I2C**: Generic platform I2C support
- **RISC-V I2C**: Platform-specific I2C controllers

## Bus Driver Integration

### Device Manager Integration
Bus drivers integrate with the MINIX device manager:
```
Device Manager
├── PCI enumeration service
├── I2C bus services
├── Device tree parsing
├── Resource allocation
└── Driver matching
```

### Service Configuration
Bus drivers are configured in `/etc/system.conf`:
```
service pci
{
    system
    {
        DEVMAN_MAP     /* Device manager integration */
        PCI_BUS        /* PCI bus access */
        IO_PORT_RANGE  /* Configuration space access */
        { 0xCF8, 0xCFF }
        MEMORY_MAPPING /* MMIO for extended config */
    }
}

service i2c
{
    system
    {
        DEVMAN_MAP     /* Device manager integration */
        I2C_BUS        /* I2C bus access */
        IRQCTL         /* Interrupt control */
        MEMORY_MAPPING /* MMIO register access */
    }
}
```

### Message Flow
Bus drivers use message passing for all communication:
```
Application → VFS → Bus Driver → Hardware
    ↓           ↓         ↓           ↑
  System Call  Message   IOCTL    Interrupt
```

## RISC-V Platform Support

### PCI Bus on RISC-V
RISC-V platforms support PCI through:
- **PCI host bridges**: Platform-specific PCI controllers
- **Configuration space**: Memory-mapped PCI config space
- **Interrupt routing**: Through Platform-Level Interrupt Controller (PLIC)
- **DMA support**: Platform DMA controllers

### I2C Bus on RISC-V
RISC-V I2C support includes:
- **Platform I2C**: SoC-integrated I2C controllers
- **Bit-banging**: GPIO-based I2C implementation
- **Device tree**: I2C device discovery via FDT
- **Interrupt support**: Through PLIC

### QEMU virt Platform
The QEMU virt machine provides:
- **PCI host bridge**: Standard PCI topology
- **I2C controllers**: Platform I2C devices
- **Device tree**: Complete hardware description
- **Interrupt routing**: Full PLIC implementation

## Power Management

### PCI Power Management
```c
/* PCI power states */
typedef enum {
    PCI_POWER_D0,    // Full power
    PCI_POWER_D1,    // Light sleep
    PCI_POWER_D2,    // Deep sleep
    PCI_POWER_D3hot, // Software off
    PCI_POWER_D3cold // Hardware off
} pci_power_state_t;

/* Power management functions */
int pci_set_power_state(int devind, pci_power_state_t state);
pci_power_state_t pci_get_power_state(int devind);
int pci_save_state(int devind);
int pci_restore_state(int devind);
```

### I2C Power Management
- **Bus power control**: Manage I2C bus power
- **Device power states**: Control attached device power
- **Suspend/resume**: Handle system power transitions
- **Clock management**: Dynamic I2C clock control

## Error Handling

### PCI Error Recovery
```c
/* PCI error types */
#define PCI_ERROR_PARITY      0x01
#define PCI_ERROR_SERR        0x02
#define PCI_ERROR_MASTER_ABORT 0x04
#define PCI_ERROR_TARGET_ABORT 0x08

/* Error recovery functions */
int pci_handle_error(int devind, int error_type);
int pci_reset_device(int devind);
int pci_restore_config(int devind);
```

### I2C Error Handling
- **Bus errors**: Arbitration loss, bus contention
- **Device errors**: NACK responses, timeout conditions
- **Protocol errors**: Invalid transfers, protocol violations
- **Recovery mechanisms**: Bus reset, retry logic

## Testing and Debugging

### PCI Testing
```bash
# PCI device enumeration
lspci -v    # List all PCI devices
lspci -t    # Show PCI bus tree

# PCI configuration space
pciconf -r  # Read configuration space
pciconf -w  # Write configuration space

# PCI resource allocation
pciconf -l  # List resource allocations
```

### I2C Testing
```bash
# I2C device scanning
i2cscan -r /dev/i2c-1  # Scan bus 1
i2cscan -r /dev/i2c-2  # Scan bus 2

# I2C device access
i2cread /dev/i2c-1 0x50  # Read from device 0x50
i2cwrite /dev/i2c-1 0x50 0x00 0x01  # Write to device

# I2C bus information
i2cinfo /dev/i2c-1     # Show bus information
```

### Debug Features
- **Register dumps**: Hardware register inspection
- **Transaction logging**: Bus transaction recording
- **Error injection**: Fault injection for testing
- **Performance metrics**: Bus utilization statistics

## Future Enhancements

### Planned Features
- **USB bus driver**: Universal Serial Bus support
- **SPI bus driver**: Serial Peripheral Interface
- **CAN bus driver**: Controller Area Network
- **Advanced PCI**: PCIe and SR-IOV support

### Architecture Improvements
- **Unified bus framework**: Common infrastructure
- **Hot-plug enhancement**: Better hot-plug support
- **Power optimization**: Advanced power management
- **Virtualization**: Bus virtualization support

## References

- **PCI Local Bus Specification**: PCI-SIG
- **I2C Bus Specification**: NXP Semiconductors
- **MINIX Device Manager**: `<minix/ds.h>`
- **System Device Framework**: `<minix/sdev.h>`
- **Character Driver Interface**: `<minix/chardriver.h>`