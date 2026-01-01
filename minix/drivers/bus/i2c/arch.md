# MINIX I2C Bus Driver Architecture

## Overview

The MINIX I2C (Inter-Integrated Circuit) bus driver provides a comprehensive framework for low-speed serial communication with peripheral devices. I2C is widely used for connecting sensors, EEPROMs, real-time clocks, and other low-bandwidth devices in embedded systems.

## I2C Protocol Architecture

### Physical Layer
- **Two-wire interface**: Serial Data (SDA) and Serial Clock (SCL)
- **Open-drain outputs**: Wired-AND bus implementation
- **Pull-up resistors**: Required on both lines
- **Multi-master capability**: Multiple controllers can arbitrate for bus
- **Clock synchronization**: SCL synchronization between masters

### Protocol Features
- **7-bit and 10-bit addressing**: Support for extended address space
- **Multiple speeds**: Standard (100 kHz), Fast (400 kHz), Fast+ (1 MHz)
- **Clock stretching**: Slave devices can slow down communication
- **General call**: Broadcast to all devices
- **START and STOP conditions**: Bus access control

### Message Types
```c
/* I2C message types */
#define I2C_M_RD        0x0001  /* Read transaction */
#define I2C_M_WR        0x0000  /* Write transaction */
#define I2C_M_TEN       0x0010  /* 10-bit address */
#define I2C_M_RECV_LEN  0x0400  /* Variable length read */
#define I2C_M_NO_RD_ACK 0x0800  /* No acknowledge on read */
#define I2C_M_IGNORE_NAK 0x1000 /* Ignore NAK */
```

## Driver Architecture

### Framework Design
The I2C driver implements a layered architecture:
```
Application Layer
├── Character device interface (/dev/i2c-*)
├── I2C IOCTL commands
└── Direct I2C access

I2C Framework Layer
├── Generic I2C driver (i2c.c)
├── Device reservation system
├── Bus arbitration logic
└── Error recovery

Hardware Abstraction Layer
├── Platform I2C controllers
├── Controller-specific drivers
├── Interrupt handling
└── DMA support (optional)
```

### Process Model
The I2C driver runs as userspace processes:
- **Main I2C driver**: Handles device requests and bus management
- **Hardware controller drivers**: Platform-specific I2C controllers
- **Device reservation**: Manages exclusive device access

## Source Code Organization

### Main Driver Files

#### `i2c.c` - Generic I2C Framework
```c
/* Core I2C driver structure */
struct i2c_driver {
    int bus_id;                    // Bus identifier
    int initialized;               // Initialization status
    struct i2cdev reservations[NR_I2CDEV]; // Device reservations
    struct log *log;               // Logging interface
    
    /* Hardware interface */
    int (*hw_process)(struct i2c_transfer *);
    int (*hw_init)(int instance);
};

Key functions:
- i2c_init()                    // Initialize I2C driver
- i2c_ioctl()                   // Handle I2C IOCTL requests
- i2c_other()                   // Handle I2C messages
- do_reserve()                  // Reserve I2C device
- do_release()                  // Release I2C device
- ds_event()                    // Handle datastore events
- sef_cb_lu_state_save()        // Save state for live update
```

### Device Reservation System
```c
/* I2C device reservation structure */
struct i2cdev {
    uint8_t inuse;                 // Reservation active
    endpoint_t endpt;              // Owner endpoint
    char key[DS_MAX_KEYLEN];       // Reservation key
};

/* Reservation functions */
static int do_reserve(endpoint_t endpt, int slave_addr);
static int check_reservation(endpoint_t endpt, int slave_addr);
static void update_reservation(endpoint_t endpt, char *key);
```

### Hardware Abstraction Interface
```c
/* I2C transfer structure */
struct i2c_transfer {
    i2c_addr_t slave_addr;         // Slave device address
    u16_t read_count;              // Bytes to read
    u16_t write_count;             // Bytes to write
    u8_t *write_buf;               // Write data buffer
    u8_t *read_buf;                // Read data buffer
    int flags;                     // Transfer flags
    int result;                    // Transfer result
};

/* Hardware interface */
int (*hw_process)(struct i2c_transfer *transfer);
int (*hw_init)(int instance);
int (*hw_set_speed)(u32_t speed);
int (*hw_reset)(void);
```

## Platform-Specific Implementations

### OMAP I2C Controller
Located in `arch/earm/omap_i2c.c`, supports AM335x/DM37xx SoCs:
```c
/* OMAP I2C controller structure */
struct omap_i2c {
    vir_bytes base;                // Register base address
    int irq;                       // Interrupt number
    int speed;                     // Bus speed (Hz)
    int fifo_size;                 // FIFO size
    
    /* Controller state */
    int busy;                      // Controller busy
    struct i2c_transfer *current;  // Current transfer
    int timeout;                   // Transfer timeout
};

Key functions:
- omap_i2c_init()               // Initialize OMAP I2C
- omap_i2c_process()            // Process I2C transfer
- omap_i2c_interrupt()          // Handle interrupts
- omap_i2c_set_speed()          // Set bus speed
- omap_i2c_reset()              // Reset controller
```

### RISC-V Platform Support
RISC-V platforms require platform-specific I2C controllers:
```c
/* RISC-V I2C controller interface */
struct riscv_i2c {
    vir_bytes base;                // MMIO register base
    int irq;                       // PLIC interrupt
    u32_t clock_freq;              // Reference clock
    int instance;                  // Controller instance
};

/* RISC-V specific functions */
int riscv_i2c_init(int instance);
int riscv_i2c_process(struct i2c_transfer *);
int riscv_i2c_set_speed(u32_t speed);
```

## Device Interface

### Character Device Access
I2C devices are accessed through character devices:
```bash
# I2C device nodes
/dev/i2c-1      # I2C bus 1
/dev/i2c-2      # I2C bus 2
/dev/i2c-3      # I2C bus 3
```

### IOCTL Commands
```c
/* I2C IOCTL definitions */
#define I2C_IOCTL_EXEC      _IOW('I', 1, struct minix_i2c_ioctl_exec)
#define I2C_IOCTL_RESERVE   _IOW('I', 2, int)
#define I2C_IOCTL_RELEASE   _IO('I', 3)
#define I2C_IOCTL_SCAN      _IOR('I', 4, struct minix_i2c_ioctl_scan)

/* I2C execution IOCTL */
struct minix_i2c_ioctl_exec {
    i2c_addr_t slave_addr;         // Slave address (7 or 10 bit)
    u16_t read_n;                  // Number of bytes to read
    u16_t write_n;                 // Number of bytes to write
    cp_grant_id_t read_grant;      // Grant ID for read buffer
    cp_grant_id_t write_grant;     // Grant ID for write buffer
    int flags;                     // Transfer flags
    int result;                    // Transfer result
};
```

### Message-Based Interface
Drivers communicate with I2C devices through messages:
```c
/* I2C message structure */
struct i2c_message {
    int m_type;                    // Message type
    i2c_addr_t slave_addr;         // Device address
    endpoint_t endpt;              // Caller endpoint
    cp_grant_id_t grant;           // Data grant
    size_t size;                   // Data size
    int flags;                     // Message flags
};

/* Message types */
#define I2C_READ_REQUEST    1
#define I2C_WRITE_REQUEST   2
#define I2C_EXEC_REQUEST    3
#define I2C_RESERVE_REQUEST 4
#define I2C_RELEASE_REQUEST 5
```

## Bus Arbitration and Multi-Master

### Arbitration Process
I2C supports multiple masters on the same bus:
```c
/* Arbitration handling */
int i2c_arbitrate_bus(struct i2c_controller *);
int i2c_release_bus(struct i2c_controller *);
int i2c_check_bus_busy(struct i2c_controller *);

/* Arbitration state */
enum i2c_arb_state {
    I2C_ARB_IDLE,                  // Bus idle
    I2C_ARB_REQUEST,               // Arbitration requested
    I2C_ARB_IN_PROGRESS,           // Arbitration in progress
    I2C_ARB_WON,                   // Arbitration won
    I2C_ARB_LOST                   // Arbitration lost
};
```

### Clock Synchronization
Multiple masters synchronize their clocks:
```c
/* Clock synchronization */
void i2c_sync_clock(struct i2c_controller *);
int i2c_detect_clock_stretch(struct i2c_controller *);
void i2c_handle_clock_stretch(struct i2c_controller *);
```

## Error Handling and Recovery

### Error Types
```c
/* I2C error codes */
#define I2C_ERROR_NO_ACK        1   // No acknowledge received
#define I2C_ERROR_ARB_LOST      2   // Arbitration lost
#define I2C_ERROR_BUS_BUSY      3   // Bus is busy
#define I2C_ERROR_TIMEOUT       4   // Transfer timeout
#define I2C_ERROR_INVALID_ADDR  5   // Invalid device address
#define I2C_ERROR_NO_MEMORY     6   // Memory allocation failed
```

### Recovery Mechanisms
```c
/* Error recovery functions */
int i2c_recover_from_error(struct i2c_controller *, int error);
int i2c_reset_bus(struct i2c_controller *);
int i2c_send_stop_condition(struct i2c_controller *);
int i2c_clear_bus(struct i2c_controller *);
```

### Timeout Handling
```c
/* Timeout management */
#define I2C_TIMEOUT_DEFAULT     1000    // 1 second
#define I2C_TIMEOUT_LONG        5000    // 5 seconds
#define I2C_TIMEOUT_SHORT       100     // 100 milliseconds

int i2c_set_timeout(struct i2c_controller *, int timeout);
int i2c_check_timeout(struct i2c_controller *);
```

## Power Management

### Bus Power States
```c
/* I2C power states */
enum i2c_power_state {
    I2C_POWER_ON,                  // Full power
    I2C_POWER_STANDBY,             // Standby mode
    I2C_POWER_SUSPEND,             // Suspend mode
    I2C_POWER_OFF                  // Powered off
};

/* Power management functions */
int i2c_set_power_state(struct i2c_controller *, enum i2c_power_state);
enum i2c_power_state i2c_get_power_state(struct i2c_controller *);
int i2c_save_state(struct i2c_controller *);
int i2c_restore_state(struct i2c_controller *);
```

### Device Power Management
- **Clock gating**: Disable I2C clock when idle
- **Pin configuration**: Set pins to low-power state
- **Wake-on-I2C**: Resume on I2C activity
- **Dynamic frequency**: Reduce clock speed for power savings

## Testing and Debugging

### Test Infrastructure
```bash
# I2C bus scanning
i2cscan -r /dev/i2c-1  # Read scan
i2cscan -w /dev/i2c-1  # Write scan

# I2C device access
i2cread /dev/i2c-1 0x50 0x00 16  # Read 16 bytes from device 0x50
i2cwrite /dev/i2c-1 0x50 0x00 0x01 0x02  # Write bytes to device

# I2C bus information
i2cinfo /dev/i2c-1     # Show bus information
```

### Debug Features
```c
/* Debug levels */
#define I2C_DEBUG_NONE      0
#define I2C_DEBUG_ERROR     1
#define I2C_DEBUG_WARN      2
#define I2C_DEBUG_INFO      3
#define I2C_DEBUG_TRACE     4

/* Debug functions */
void i2c_debug_transfer(struct i2c_transfer *);
void i2c_dump_registers(struct i2c_controller *);
void i2c_trace_bus_state(struct i2c_controller *);
```

## RISC-V Platform Integration

### Device Tree Support
RISC-V platforms use device tree for I2C configuration:
```dts
i2c0: i2c@10016000 {
    compatible = "sifive,i2c0";
    reg = <0x0 0x10016000 0x0 0x1000>;
    interrupts = <5>;
    clocks = <&tlclk>;
    clock-frequency = <100000>;
    status = "okay";
    
    eeprom@50 {
        compatible = "atmel,24c256";
        reg = <0x50>;
    };
};
```

### PLIC Integration
I2C interrupts are routed through the Platform-Level Interrupt Controller:
```c
/* RISC-V I2C interrupt handling */
int riscv_i2c_setup_interrupt(struct riscv_i2c *, int irq);
void riscv_i2c_interrupt_handler(struct riscv_i2c *);
int riscv_i2c_enable_interrupt(struct riscv_i2c *, int mask);
int riscv_i2c_disable_interrupt(struct riscv_i2c *, int mask);
```

### Memory-Mapped I/O
RISC-V I2C controllers use memory-mapped registers:
```c
/* RISC-V I2C register access */
static inline u32_t riscv_i2c_read(struct riscv_i2c *i2c, int offset)
{
    return mmio_read(i2c->base + offset);
}

static inline void riscv_i2c_write(struct riscv_i2c *i2c, int offset, u32_t value)
{
    mmio_write(i2c->base + offset, value);
}
```

## Performance Optimization

### Transfer Optimization
- **Combined transfers**: Read after write in single transaction
- **Block transfers**: Transfer multiple bytes efficiently
- **FIFO utilization**: Use hardware FIFO when available
- **DMA support**: Platform DMA for large transfers

### Bus Utilization
- **Clock speed optimization**: Use highest reliable speed
- **Bus arbitration**: Minimize arbitration overhead
- **Transaction pipelining**: Overlap transactions when possible
- **Error recovery**: Fast error detection and recovery

## Common I2C Devices

### Temperature Sensors
- **LM75**: Digital temperature sensor
- **DS1621**: Thermometer/thermostat
- **BMP085**: Pressure and temperature sensor

### EEPROMs
- **24C02**: 2K-bit serial EEPROM
- **24C256**: 256K-bit serial EEPROM
- **AT24C32**: 32K-bit serial EEPROM

### Real-Time Clocks
- **DS1307**: Serial real-time clock
- **PCF8563**: CMOS real-time clock
- **RV-3028-C7**: Temperature-compensated RTC

### I/O Expanders
- **PCF8574**: 8-bit I/O expander
- **MCP23017**: 16-bit I/O expander
- **PCA9555**: 16-bit I/O expander

## Future Enhancements

### Planned Features
- **SPI bus driver**: Serial Peripheral Interface support
- **SMBus support**: System Management Bus protocols
- **Advanced power management**: Aggressive power saving
- **Multi-controller support**: Enhanced multi-master arbitration

### Architecture Improvements
- **Unified serial framework**: Common infrastructure for I2C/SPI/UART
- **Plugin architecture**: Modular controller support
- **Advanced debugging**: Logic analyzer integration
- **Performance monitoring**: Detailed bus statistics

## References

- **I2C Bus Specification**: NXP Semiconductors
- **System Management Bus Specification**: SBS Implementers Forum
- **MINIX Character Driver Framework**: `<minix/chardriver.h>`
- **Device Manager Interface**: `<minix/ds.h>`
- **OMAP I2C Controller Guide**: Texas Instruments