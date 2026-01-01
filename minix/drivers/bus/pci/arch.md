# MINIX PCI Bus Driver Architecture

## Overview

The MINIX PCI (Peripheral Component Interconnect) bus driver provides comprehensive device enumeration, resource allocation, and access control for PCI devices. As the primary system bus in modern computers, PCI serves as the foundation for connecting high-performance peripherals including network interfaces, storage controllers, graphics cards, and audio devices.

## PCI Architecture Fundamentals

### Bus Hierarchy
PCI implements a tree-structured hierarchy:
```
Host Bridge (Root Complex)
├── PCI Bus 0
│   ├── PCI Devices (00:00.0, 00:01.0, etc.)
│   ├── PCI-to-PCI Bridge
│   │   └── PCI Bus 1
│   │       ├── PCI Devices (01:00.0, 01:01.0, etc.)
│   │       └── PCI-to-PCI Bridge
│   │           └── PCI Bus 2
│   └── PCI-to-ISA Bridge
│       └── ISA Bus (Legacy devices)
└── PCIe Root Ports (PCI Express)
    ├── PCIe Devices
    └── PCIe Switches
```

### Configuration Space
Each PCI device implements a 256-byte configuration space:
```
Offset 0x00: Vendor ID (16-bit) | Device ID (16-bit)
Offset 0x04: Command (16-bit) | Status (16-bit)
Offset 0x08: Class Code (24-bit) | Revision ID (8-bit)
Offset 0x0C: BIST | Header Type | Latency Timer | Cache Line Size
Offset 0x10: Base Address Register 0 (BAR0)
Offset 0x14: Base Address Register 1 (BAR1)
Offset 0x18: Base Address Register 2 (BAR2)
Offset 0x1C: Base Address Register 3 (BAR3)
Offset 0x20: Base Address Register 4 (BAR4)
Offset 0x24: Base Address Register 5 (BAR5)
Offset 0x28: CardBus CIS Pointer
Offset 0x2C: Subsystem Vendor ID | Subsystem ID
Offset 0x30: Expansion ROM Base Address
Offset 0x34: Reserved | Capabilities Pointer
Offset 0x38: Reserved
Offset 0x3C: Max Latency | Min Grant | Interrupt Pin | Interrupt Line
```

## Driver Architecture

### Design Principles
The PCI driver follows MINIX microkernel principles:
- **Userspace implementation**: Runs as regular process
- **Message-based IPC**: All communication via messages
- **Resource management**: Centralized allocation
- **Access control**: Permission-based device access
- **Fault isolation**: Driver failures don't crash kernel

### Process Model
```
PCI Driver Process
├── Main Message Loop
├── Device Enumeration Thread
├── Resource Management
├── Access Control
└── Hot-plug Handling
```

## Source Code Organization

### Core Files

#### `main.c` - PCI Driver Entry Point
```c
/* Main driver functions */
int main(void);                    // Driver initialization and main loop
static void sef_cb_init_fresh(void); // Fresh initialization callback
static int sef_cb_init_lu(void);    // Live update initialization

/* Message handlers */
static void do_init(message *mp);           // Initialize PCI subsystem
static void do_first_dev(message *mp);      // Get first matching device
static void do_next_dev(message *mp);       // Get next matching device
static void do_find_dev(message *mp);       // Find specific device
static void do_ids(message *mp);            // Get device IDs
static void do_get_bar(message *mp);        // Get Base Address Register
static void do_get_irq(message *mp);        // Get IRQ assignment
static void do_reserve(message *mp);        // Reserve device
static void do_release(message *mp);         // Release device
static void do_get_acl(message *mp);        // Get access control list
static void do_set_acl(message *mp);        // Set access control list
```

#### `pci.c` - PCI Implementation Core
```c
/* PCI subsystem initialization */
void pci_init(void);               // Initialize PCI subsystem
int pci_first_dev(struct rs_pci *aclp, int *devindp, u16_t *vidp, u16_t *didp);
int pci_next_dev(struct rs_pci *aclp, int *devindp, u16_t *vidp, u16_t *didp);
int pci_find_dev(u8_t bus, u8_t dev, u8_t func, int *devindp);

/* Device access functions */
int pci_get_bar(int devind, int bar_nr, u32_t *basep, u32_t *sizep);
int pci_get_irq(int devind);
int pci_reserve_dev(struct rs_pci *aclp, int devind);
void pci_release_dev(endpoint_t proc);

/* Configuration space access */
u8_t pci_attr_r8(int devind, int port);
u16_t pci_attr_r16(int devind, int port);
u32_t pci_attr_r32(int devind, int port);
void pci_attr_w8(int devind, int port, u8_t value);
void pci_attr_w16(int devind, int port, u16_t value);
void pci_attr_w32(int devind, int port, u32_t value);

/* Bridge handling */
void pci_handle_bridge(int devind);
void pci_handle_cardbus(int devind);
void pci_handle_isa_bridge(int devind);

/* Resource management */
int pci_alloc_irq(int bus, int dev, int func);
int pci_alloc_io_range(u32_t size, u32_t align);
int pci_alloc_mem_range(u32_t size, u32_t align);
```

#### `pci.h` - PCI Driver Header
Contains type definitions, function prototypes, and configuration constants.

#### `pci_table.c` - PCI Device Database
Contains known PCI device identifiers and compatibility information.

## Device Enumeration Process

### Bus Scanning Algorithm
```c
/* PCI enumeration process */
void pci_scan_buses(void)
{
    int bus, dev, func;
    
    for (bus = 0; bus < NR_PCI_BUSES; bus++) {
        for (dev = 0; dev < NR_PCI_DEVICES; dev++) {
            for (func = 0; func < NR_PCI_FUNCTIONS; func++) {
                pci_scan_function(bus, dev, func);
            }
        }
    }
}

/* Function scanning */
void pci_scan_function(u8_t bus, u8_t dev, u8_t func)
{
    u16_t vendor, device;
    u8_t header_type;
    u8_t class, subclass;
    
    /* Read device identification */
    vendor = pci_attr_r16(devind, PCI_VID);
    device = pci_attr_r16(devind, PCI_DID);
    
    /* Skip non-existent devices */
    if (vendor == 0xFFFF) return;
    
    /* Read device class and header type */
    class = pci_attr_r8(devind, PCI_CLASS);
    subclass = pci_attr_r8(devind, PCI_SUBCLASS);
    header_type = pci_attr_r8(devind, PCI_HEADERTYPE);
    
    /* Handle different device types */
    if (class == PCI_CLASS_BRIDGE && subclass == PCI_SUBCLASS_BRIDGE_PCI) {
        pci_handle_bridge(devind);
    } else if (class == PCI_CLASS_BRIDGE && subclass == PCI_SUBCLASS_BRIDGE_CARDBUS) {
        pci_handle_cardbus(devind);
    } else if (class == PCI_CLASS_BRIDGE && subclass == PCI_SUBCLASS_BRIDGE_ISA) {
        pci_handle_isa_bridge(devind);
    } else {
        /* Regular device */
        pci_add_device(devind);
    }
    
    /* Check for multi-function device */
    if (func == 0 && (header_type & PCI_MULTIFUNCTION)) {
        /* Device has multiple functions */
        for (func = 1; func < NR_PCI_FUNCTIONS; func++) {
            pci_scan_function(bus, dev, func);
        }
    }
}
```

### Bridge Handling
```c
/* PCI-to-PCI bridge handling */
void pci_handle_bridge(int devind)
{
    u8_t pri_bus, sec_bus, sub_bus;
    u16_t bridge_control;
    
    /* Read bridge configuration */
    pri_bus = pci_attr_r8(devind, PCI_PRI_BUS);
    sec_bus = pci_attr_r8(devind, PCI_SEC_BUS);
    sub_bus = pci_attr_r8(devind, PCI_SUB_BUS);
    
    /* Configure bridge */
    if (sec_bus == 0) {
        /* Assign bus numbers */
        sec_bus = pci_alloc_bus();
        sub_bus = sec_bus;
        pci_attr_w8(devind, PCI_SEC_BUS, sec_bus);
        pci_attr_w8(devind, PCI_SUB_BUS, sub_bus);
    }
    
    /* Scan secondary bus */
    pci_scan_bus(sec_bus);
}
```

## Resource Management

### Base Address Registers (BARs)
```c
/* BAR handling */
int pci_get_bar(int devind, int bar_nr, u32_t *basep, u32_t *sizep)
{
    u32_t bar, mask;
    u32_t base, size;
    int is_io;
    
    /* Read BAR */
    bar = pci_attr_r32(devind, PCI_BAR + bar_nr * 4);
    
    /* Determine BAR type */
    is_io = (bar & PCI_BAR_IO) ? 1 : 0;
    
    if (is_io) {
        /* I/O space BAR */
        mask = PCI_BAR_IO_MASK;
        base = bar & mask;
        
        /* Calculate size */
        pci_attr_w32(devind, PCI_BAR + bar_nr * 4, mask);
        size = ~pci_attr_r32(devind, PCI_BAR + bar_nr * 4) & mask;
        pci_attr_w32(devind, PCI_BAR + bar_nr * 4, bar);
    } else {
        /* Memory space BAR */
        mask = PCI_BAR_MEM_MASK;
        base = bar & mask;
        
        /* Calculate size */
        pci_attr_w32(devind, PCI_BAR + bar_nr * 4, mask);
        size = ~pci_attr_r32(devind, PCI_BAR + bar_nr * 4) & mask;
        pci_attr_w32(devind, PCI_BAR + bar_nr * 4, bar);
    }
    
    *basep = base;
    *sizep = size + 1;
    
    return is_io ? PCI_BAR_TYPE_IO : PCI_BAR_TYPE_MEM;
}
```

### IRQ Allocation
```c
/* IRQ assignment */
int pci_alloc_irq(int bus, int dev, int func)
{
    static int next_irq = 5;  // Start from IRQ 5
    int irq;
    
    /* Allocate next available IRQ */
    irq = next_irq++;
    
    /* Update device configuration */
    pci_attr_w8(devind, PCI_ILINE, irq);
    
    return irq;
}
```

## Access Control System

### ACL Management
```c
/* PCI access control list */
struct pci_acl {
    int inuse;                     // Entry in use
    struct rs_pci acl;            // Access control data
};

/* ACL operations */
int pci_grant_access(int devind, endpoint_t proc);
int pci_revoke_access(endpoint_t proc);
struct rs_pci *pci_find_acl(endpoint_t proc);
```

### Device Reservation
```c
/* Device reservation process */
int pci_reserve_dev(struct rs_pci *aclp, int devind)
{
    struct pci_dev *pdev;
    
    /* Find device */
    pdev = &pci_dev[devind];
    if (!pdev->present) return ENOENT;
    
    /* Check if already reserved */
    if (pdev->reserved) return EBUSY;
    
    /* Check access permissions */
    if (!pci_check_access(aclp, pdev)) return EPERM;
    
    /* Reserve device */
    pdev->reserved = 1;
    pdev->owner = aclp->rsp_endpoint;
    
    /* Return device information */
    return OK;
}
```

## Message-Based Interface

### Driver Communication
PCI driver uses message passing for all communication:
```c
/* PCI message types */
#define PCI_INIT            1
#define PCI_FIRST_DEV       2
#define PCI_NEXT_DEV        3
#define PCI_FIND_DEV        4
#define PCI_IDS             5
#define PCI_GET_BAR         6
#define PCI_GET_IRQ         7
#define PCI_RESERVE         8
#define PCI_RELEASE         9
#define PCI_GET_ACL         10
#define PCI_SET_ACL         11

/* Message structures */
typedef struct {
    int m_type;
    endpoint_t m_source;
    
    union {
        /* Device enumeration */
        struct {
            int devind;
            u16_t vid;
            u16_t did;
        } dev_info;
        
        /* BAR access */
        struct {
            int devind;
            int bar_nr;
            u32_t base;
            u32_t size;
        } bar_info;
        
        /* IRQ access */
        struct {
            int devind;
            int irq;
        } irq_info;
    } u;
} pci_message_t;
```

### System Service Configuration
PCI driver configuration in `/etc/system.conf`:
```
service pci
{
    system
    {
        DEVMAN_MAP     /* Device manager integration */
        PCI_BUS        /* PCI bus access */
        IO_PORT_RANGE  /* Configuration space access */
        { 0xCF8, 0xCFF }
        MEMORY_MAPPING /* Extended configuration space */
        PRIVCTL        /* System privilege control */
    }
}
```

## RISC-V Platform Support

### PCI Host Bridge
RISC-V platforms implement PCI through host bridges:
```c
/* RISC-V PCI host bridge */
struct riscv_pci_host {
    vir_bytes cfg_base;            // Configuration space base
    vir_bytes mem_base;            // Memory space base
    vir_bytes io_base;             // I/O space base
    int first_bus;                 // First bus number
    int last_bus;                  // Last bus number
    int irq_base;                  // Base IRQ number
};

/* RISC-V PCI functions */
int riscv_pci_init(struct riscv_pci_host *host);
int riscv_pci_read_config(int bus, int dev, int func, int reg, int size, u32_t *value);
int riscv_pci_write_config(int bus, int dev, int func, int reg, int size, u32_t value);
```

### Memory-Mapped Configuration
RISC-V uses memory-mapped PCI configuration:
```c
/* Configuration space access */
static int riscv_pci_read_config(int bus, int dev, int func, int reg, int size, u32_t *value)
{
    vir_bytes addr;
    
    /* Build configuration address */
    addr = PCI_CFG_ADDR(bus, dev, func, reg);
    
    /* Read configuration data */
    switch (size) {
    case 1:
        *value = mmio_read8(addr);
        break;
    case 2:
        *value = mmio_read16(addr);
        break;
    case 4:
        *value = mmio_read32(addr);
        break;
    }
    
    return OK;
}
```

### Interrupt Handling
PCI interrupts are routed through PLIC:
```c
/* RISC-V PCI interrupt setup */
int riscv_pci_setup_interrupt(int devind, int irq)
{
    struct riscv_pci_host *host = &riscv_pci_host;
    int plic_irq;
    
    /* Map PCI IRQ to PLIC interrupt */
    plic_irq = host->irq_base + irq;
    
    /* Configure PLIC */
    plic_enable_interrupt(plic_irq);
    plic_set_priority(plic_irq, PCI_IRQ_PRIORITY);
    
    return plic_irq;
}
```

## Hot-Plug Support

### PCI Hot-Plug
```c
/* Hot-plug event handling */
void pci_handle_hotplug_event(int bus, int slot)
{
    struct pci_hotplug_event event;
    
    /* Determine event type */
    if (pci_slot_occupied(bus, slot)) {
        /* Device insertion */
        pci_enumerate_slot(bus, slot);
        pci_notify_drivers(PCI_EVENT_DEVICE_ADDED, bus, slot);
    } else {
        /* Device removal */
        pci_notify_drivers(PCI_EVENT_DEVICE_REMOVED, bus, slot);
        pci_remove_device(bus, slot);
    }
}
```

### Power Management
```c
/* PCI power management */
typedef enum {
    PCI_POWER_D0,    // Full power
    PCI_POWER_D1,    // Light sleep
    PCI_POWER_D2,    // Deep sleep
    PCI_POWER_D3hot, // Software off
    PCI_POWER_D3cold // Hardware off
} pci_power_state_t;

int pci_set_power_state(int devind, pci_power_state_t state);
pci_power_state_t pci_get_power_state(int devind);
```

## Error Handling

### PCI Error Types
```c
/* PCI error conditions */
#define PCI_ERROR_PARITY      0x01  // Parity error
#define PCI_ERROR_SERR        0x02  // System error
#define PCI_ERROR_MASTER_ABORT 0x04 // Master abort
#define PCI_ERROR_TARGET_ABORT 0x08 // Target abort
#define PCI_ERROR_SIG_TARGET_ABORT 0x10 // Signaled target abort
#define PCI_ERROR_DEVSEL_TIMEOUT 0x20 // DEVSEL timeout
#define PCI_ERROR_MASTER_PARITY 0x40 // Master parity error
```

### Error Recovery
```c
/* Error recovery functions */
int pci_handle_error(int devind, int error_type);
int pci_reset_device(int devind);
int pci_restore_config(int devind);
int pci_disable_device(int devind);
```

## Testing and Debugging

### PCI Utilities
```bash
# PCI device enumeration
lspci -v    # List all PCI devices with details
lspci -t    # Show PCI bus tree structure
lspci -n    # Show numeric IDs

# PCI configuration space
pciconf -r -b 0:0:0 0x00 0x40  # Read 64 bytes from bus 0, dev 0, func 0
pciconf -w -b 0:0:0 0x04 0x06   # Write command register

# PCI resource allocation
pciconf -l    # List resource allocations
pciconf -a    # Show allocated resources
```

### Debug Features
```c
/* PCI debugging */
#define PCI_DEBUG_ENUMERATION 0x01
#define PCI_DEBUG_RESOURCES   0x02
#define PCI_DEBUG_ACCESS      0x04
#define PCI_DEBUG_HOTPLUG     0x08

extern int pci_debug;

#define pci_debug(level, fmt, ...) \
    do { if (pci_debug & (level)) printf(fmt, ##__VA_ARGS__); } while (0)
```

## Performance Optimization

### Enumeration Optimization
- **Cached enumeration**: Store device information
- **Selective scanning**: Skip empty bus segments
- **Parallel enumeration**: Scan multiple buses concurrently
- **Incremental updates**: Only scan changed devices

### Access Optimization
- **Batch operations**: Combine multiple configuration accesses
- **Cached reads**: Cache frequently accessed registers
- **Direct hardware access**: Bypass kernel for performance
- **Interrupt coalescing**: Reduce interrupt overhead

## Future Enhancements

### Planned Features
- **PCI Express support**: Native PCIe enumeration
- **SR-IOV support**: Single Root I/O Virtualization
- **Advanced error handling**: AER (Advanced Error Reporting)
- **ATS support**: Address Translation Services

### Architecture Improvements
- **Unified bus framework**: Common infrastructure for all buses
- **Enhanced hot-plug**: Better hot-plug and surprise removal
- **Power management**: Advanced power state management
- **Virtualization**: IOMMU and virtualization support

## References

- **PCI Local Bus Specification**: PCI-SIG
- **PCI Express Base Specification**: PCI-SIG
- **MINIX Resource Manager**: `<minix/rs.h>`
- **Device Manager Interface**: `<minix/ds.h>`
- **System Event Framework**: `<minix/sef.h>`